"""ImageWriter MCP server.

A stdio-based Model Context Protocol server. The Claude host app spawns this
process on startup and communicates with it over stdin/stdout, so nothing here
should print to stdout directly (that stream is the protocol channel). Use the
`logging` module, which writes to stderr, for any diagnostics.
"""

import socket
import pythonPackage as iw

import logging
import ctypes
from ctypes import *
from typing import Annotated, Literal
from pydantic import Field
from fastmcp import FastMCP
from fastmcp.exceptions import ToolError

# Logs go to stderr; stdout is reserved for the MCP protocol.
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger("imagewriter")


mcp = FastMCP(
    "ImageWriter",
    instructions=(
        "This server connects to an image writer application and tells it to draw whatever the user wishes. "
        "With every shape you draw, you specify a Transform describing its position, orientation and scale "
        "in the 2D world, in addition you can also query the camera Transform to see what the user is currently looking at"
    ),
)

def connectToImageWriterApp() -> bool:

    global connToServer
    
    try:
        connToServer = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        connToServer.connect((iw.HOST_IP, iw.PORT))
        logger.info(f"Establised connection to image writer app!")
    except (TimeoutError, ConnectionRefusedError) as e:
        logger.info(f"Connection attempt failed: {e}\nTrying again...")
        connToServer.close()
        return False
    
    return True

def disconnectFromImageWriter():

    commIn = iw.Command.Disconnecting
    commandBuffer = bytes(commIn.value.to_bytes(iw.COMMAND_SIZE))

    try:
        connToServer.sendall(commandBuffer)
    except ConnectionResetError as e:
        logger.error(f"Note: disconnecting failed: ConnectionResetError: {e}\n\n")
    finally:
        connToServer.close()

def sendCommandToApp(command: iw.Command, params, ReplyType: type):

    logger.info(f"To server: {params}")

    commandBuffer = bytes(command.value.to_bytes(iw.COMMAND_SIZE))

    
    if not connectToImageWriterApp():
        raise ToolError("sendCommandToApp(): Failed to connect to ImageWriter app")

    # send message to server
    try:
        connToServer.sendall(commandBuffer)

        # if command has params, serialize and send
        if params is not None:
            paramsBuffer = bytes(params)
            connToServer.sendall(paramsBuffer)

    # will fail if server had disconnected at time of sending message
    except ConnectionResetError as e:
        logger.error(f"ConnectionResetError: {e}\n\n")
        disconnectFromImageWriter()
        raise ToolError("sendCommandToApp(): Failed to send command to ImageWriter app")

    replyBuffer = bytearray()
    
    # wait here and receive message from client
    success, errorMessage = iw.receiveMessage(buffer = replyBuffer, connection = connToServer, size = ctypes.sizeof(ReplyType))
    if not success:
        # log error message and return to connecting state
        logger.error(errorMessage)
        disconnectFromImageWriter()
        raise ToolError("sendCommandToApp(): Failed to receive reply from ImageWriter app")
    
    # deserialize client message and log
    reply = ReplyType.from_buffer_copy(replyBuffer)
    logger.info(f"From server: {reply}")

    disconnectFromImageWriter()

    return reply

@mcp.tool()
def setCameraTransform(
    transform: Annotated[iw.TransformMCPPayload, Field(description = f"specifies the new transform of the camera.")],
) -> iw.PlainReplyMCPPayload:
    """sets the transform of the camera, note the scale determines the camera view rectangle's half Y extent
    where the halfX extent is then internally determined by the screen aspect ratio
    """

    transformRaw = iw.Transform()
    transformRaw.fromMCPPayload(transform)

    logger.info(f"To server: {transformRaw}")

    return sendCommandToApp(iw.Command.SetCameraTransform, transformRaw, iw.PlainReply).toMCPPayload()

@mcp.tool()
def getCameraTransform() -> iw.CameraTransformReplyMCPPayload:
    """
    returns the rectangle representing the camera's world transform. Note that the "scale" parameter denotes the
    half Y extent of the camera in world units where the half X extent can be obtained from multiplying that by the 
    returned widthFromHeight value
    """

    return sendCommandToApp(iw.Command.GetCameraTransform, None, iw.TransformReply).toCameraMCPPayload()

@mcp.tool()
def addEntity(
    transform: Annotated[iw.TransformMCPPayload, Field(description = f"specifies how to place this new entity.")],
    color: Annotated[iw.ColorMCPPayload, Field(description = f"specifies the color to draw this shape")],
    zOrder: Annotated[int, Field(description = f"the Z order, {iw.Z_ORDER_DOC}")],
    shape: Annotated[iw.ShapeDataUnionMCPPayload, Field(description = iw.SHAPE_DATA_UNION_DOC)]
):

    """
    Places a new entity within the world at specified transform and color
    """
    entityParams = iw.EntityData()
    entityParams.fromAddEntityMCPPayload(transform, color, zOrder, shape)
    
    return sendCommandToApp(iw.Command.AddEntity, entityParams, iw.AddEntityReply).toMCPPayload()

@mcp.tool()
def modifyEntity4(
    entityId: Annotated[int, Field(description = "The id of the entity to modify")],
    shapeType: Annotated[iw.EntityTypeMCPPayload, Field(description = "an optional new shape to set for the entity")] = None,
    position: Annotated[iw.ModifyPositionModel, Field(description = "an optional new position of the transform to set for the entity")] = None,
    orientation: Annotated[iw.ModifyOrientationModel, Field(description = "an optional new orientation of the transform to set for the entity")] = None,
    scale: Annotated[iw.ModifyScaleModel, Field(description = "an optional new scale of the transform to set for the entity")] = None,
    color: Annotated[iw.ColorMCPPayload, Field(description = "an optional new color to set for the entity")] = None,
    zOrder: Annotated[int, Field(description = f"an optional new zOrder to set for the entity, {iw.Z_ORDER_DOC}")] = None,
    halfExtents: Annotated[iw.Vec2MCPPayload, Field(description = "optional new half extents to set for an ellipse/rectangle entity")] = None,
    trianglePoint1: Annotated[iw.Vec2MCPPayload, Field(description = "optional new 'point1' to set for a triangle entity")] = None,
    trianglePoint2: Annotated[iw.Vec2MCPPayload, Field(description = "optional new 'point2' to set for a triangle entity")] = None,
    trianglePoint3: Annotated[iw.Vec2MCPPayload, Field(description = "optional new 'point3' to set for a triangle entity")] = None,
):
    """
    Use this function to modify various properties of existing entities given its id. All fields apart from 'entityId' are optional
    meaning they remain the same if you don't provide values for them. The shape specific parameters {halfExtents, trianglePoint<N>}
    will only be applied if the existing entity is of that shape, you will receive an error message if not. To change the shape entirely
    with fresh parameters, use the 'shapeType' parameter, you must then provide its shape specific parameters.
    """

    entityParams = iw.EntityData()
    reply = entityParams.fromModifyEntityMCPPayload(entityId, shapeType, position, orientation, scale, color, zOrder, halfExtents, trianglePoint1, trianglePoint2, trianglePoint3)
    if not reply.success:
        return reply

    return sendCommandToApp(iw.Command.ModifyEntity, entityParams, iw.PlainReply).toMCPPayload()
    #return iw.PlainReplyMCPPayload(success=True, message=f"{entityParams}")

@mcp.tool()
def getEntityData(
    entityId: Annotated[int, Field(description = "The id of the entity to get info on")],
):
    """
    Returns data on an entity given its id
    """

    entityIdRaw = ctypes.c_uint64(entityId)
    return sendCommandToApp(iw.Command.GetEntityData, entityIdRaw, iw.EntityDataReply).toMCPPayload()

@mcp.tool()
def queryScene_PROTO():
    
    fetchNumEntitiesReply = sendCommandToApp(iw.Command.GetNumEntities, None, iw.NumEntitiesReply)

    if not fetchNumEntitiesReply.success:
        return iw.PlainReplyMCPPayload(success = False, message = fetchNumEntitiesReply.message.decode('utf-8'))

    class AllEntitiesDataReply(ctypes.Structure):
        _fields_ = [
            ("success", ctypes.c_bool),
            ("message", ctypes.c_char * iw.MAX_REPLY_MESSAGE_LENGTH),
            ("array", iw.EntityData * fetchNumEntitiesReply.numEntities),
        ]

        def __str__(self):
            return f"(AllEntitiesDataReply: success = {self.success}, message = {self.message.decode('utf-8')}, array = <Entity Data>)"

    allEntitiesDataReply = sendCommandToApp(iw.Command.GetAllEntityData, ctypes.c_uint64(fetchNumEntitiesReply.numEntities), AllEntitiesDataReply)
    
    if not allEntitiesDataReply.success:
        return iw.PlainReplyMCPPayload(success = False, message = allEntitiesDataReply.message.decode('utf-8'))

    parsedEntities = []
    for entityRaw in allEntitiesDataReply.array:
        if entityRaw.entityType == iw.EntityType.ELLIPSE:
            entityModel = {
                "entityId": entityRaw.entityIdToModify, 
                "shape": "ellipse",
                "transform": {"position": {"x": entityRaw.transform.position.x, "y": entityRaw.transform.position.y}, "orientation": entityRaw.transform.orientation, "scale": entityRaw.transform.scale},
                "color": {"red": entityRaw.color.red, "green": entityRaw.color.green, "blue": entityRaw.color.blue, "alpha": entityRaw.color.alpha},
                "zOrder": entityRaw.zOrder,
                "halfExtents": {"x": entityRaw.ellipse.halfExtents.x, "y": entityRaw.ellipse.halfExtents.y},
            }
            parsedEntities.append(entityModel)
        elif entityRaw.entityType == iw.EntityType.RECTANGLE:
            entityModel = {
                "entityId": entityRaw.entityIdToModify, 
                "shape": "rectangle",
                "transform": {"position": {"x": entityRaw.transform.position.x, "y": entityRaw.transform.position.y}, "orientation": entityRaw.transform.orientation, "scale": entityRaw.transform.scale},
                "color": {"red": entityRaw.color.red, "green": entityRaw.color.green, "blue": entityRaw.color.blue, "alpha": entityRaw.color.alpha},
                "zOrder": entityRaw.zOrder,
                "halfExtents": {"x": entityRaw.rectangle.halfExtents.x, "y": entityRaw.rectangle.halfExtents.y},
            }
            parsedEntities.append(entityModel)
        elif entityRaw.entityType == iw.EntityType.TRIANGLE:
            entityModel = {
                "entityId": entityRaw.entityIdToModify, 
                "shape": "triangle",
                "transform": {"position": {"x": entityRaw.transform.position.x, "y": entityRaw.transform.position.y}, "orientation": entityRaw.transform.orientation, "scale": entityRaw.transform.scale},
                "color": {"red": entityRaw.color.red, "green": entityRaw.color.green, "blue": entityRaw.color.blue, "alpha": entityRaw.color.alpha},
                "zOrder": entityRaw.zOrder,
                "point1": {"x": entityRaw.triangle.point1.x, "y": entityRaw.triangle.point1.y},
                "point2": {"x": entityRaw.triangle.point2.x, "y": entityRaw.triangle.point2.y},
                "point3": {"x": entityRaw.triangle.point3.x, "y": entityRaw.triangle.point3.y},
            }
            parsedEntities.append(entityModel)

    ret = iw.SceneDataReplyModel(success = True, message= "All entities obtained successfully", numEntities = fetchNumEntitiesReply.numEntities, entities = parsedEntities)
    return ret

@mcp.tool()
def removeEntity(
    entityId: Annotated[int, Field(description = "The id of the entity to remove")],
) -> iw.PlainReplyMCPPayload:
    """
    Allows entity removal given its id
    """

    entityIdRaw = ctypes.c_uint64(entityId)
    return sendCommandToApp(iw.Command.RemoveEntity, entityIdRaw, iw.PlainReply).toMCPPayload()

@mcp.tool()
def removeAllEntities() -> iw.PlainReplyMCPPayload:
    """clears the world of all spawned entities"""

    return sendCommandToApp(iw.Command.RemoveAllEntities, None, iw.PlainReply).toMCPPayload()

if __name__ == "__main__":

    mcp.run()
