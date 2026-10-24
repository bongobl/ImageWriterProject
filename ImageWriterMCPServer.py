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
from typing import Annotated
from pydantic import Field
from mcp.server.mcpserver import MCPServer
from mcp.server.mcpserver.exceptions import ToolError

# Logs go to stderr; stdout is reserved for the MCP protocol.
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger("imagewriter")


mcp = MCPServer(
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
    transform: Annotated[iw.TransformMCPPayload, Field(description = f"specifies the new transform of the camera. {iw.TRANSFORM_DOC}")],
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
    """returns the rectangle representing the camera's world transform. Note that the "scale" parameter denotes the
    half Y extent of the camera in world units where the half X extent can be obtained from multiplying that by the 
    returned widthFromHeight value
  
    """

    return sendCommandToApp(iw.Command.GetCameraTransform, None, iw.TransformReply).toCameraMCPPayload()

@mcp.tool()
def removeAllEntities() -> iw.PlainReplyMCPPayload:
    """clears the world of all spawned entities"""

    return sendCommandToApp(iw.Command.RemoveAllEntities, None, iw.PlainReply).toMCPPayload()

@mcp.tool()
def addEllipseEntity(
        transform: Annotated[iw.TransformMCPPayload, Field(description = f"specifies how to place this new ellipse. {iw.TRANSFORM_DOC}")],
        color: Annotated[iw.ColorMCPPayload, Field(description = f"specifies the color to draw this shape. {iw.COLOR_DOC}")],
        zOrder: Annotated[int, Field(description = f"the Z order {iw.Z_ORDER_DOC}")],
        halfExtents: Annotated[iw.Vec2MCPPayload, Field(description = f"half extents of the ellipse in its local space. {iw.VEC2_DOC}")],
) -> iw.AddEntityReplyMCPPayload:
    """
    Places a new ellipse within the world at specified transform and color
    Note that the half extents define the shape's size in its own local space, where its global size is then obtained by multiplying
    the extents by the shape's scale transform
    """

    # create addEllipse command params
    transformRaw = iw.Transform()
    transformRaw.fromMCPPayload(transform)

    colorRaw = iw.Color()
    colorRaw.fromMCPPayload(color)

    zOrderRaw = ctypes.c_int(zOrder)

    halfExtentsRaw = iw.Vec2()
    halfExtentsRaw.fromMCPPayload(halfExtents)

    ellipseParams = iw.AddEntityParams(
        entityType = iw.EntityType.ELLIPSE, 
        transform = transformRaw,
        color = colorRaw,
        zOrder = zOrderRaw,
        ellipse = iw.AddEllipseParams(halfExtents = halfExtentsRaw)
    )

    return sendCommandToApp(iw.Command.AddEntity, ellipseParams, iw.AddEntityReply).toMCPPayload()
    
@mcp.tool()
def addRectangleEntity(
    transform: Annotated[iw.TransformMCPPayload, Field(description = f"specifies how to place this new rectangle. {iw.TRANSFORM_DOC}")],
    color: Annotated[iw.ColorMCPPayload, Field(description = f"specifies the color to draw this shape. {iw.COLOR_DOC}")],
    zOrder: Annotated[int, Field(description = f"the Z order {iw.Z_ORDER_DOC}")],
    halfExtents: Annotated[iw.Vec2MCPPayload, Field(description = f"half extents of the rectangle in its local space. {iw.VEC2_DOC}")],
) -> iw.AddEntityReplyMCPPayload:
    """
    Places a new rectangle within the world at specified transform and color
    Note that the half extents define the shape's size in its own local space, where its global size is then obtained by multiplying
    the extents by the shape's scale transform
    """


    # create addRectangle command params
    transformRaw = iw.Transform()
    transformRaw.fromMCPPayload(transform)

    colorRaw = iw.Color()
    colorRaw.fromMCPPayload(color)

    zOrderRaw = ctypes.c_int(zOrder)

    halfExtentsRaw = iw.Vec2()
    halfExtentsRaw.fromMCPPayload(halfExtents)

    rectangleParams = iw.AddEntityParams(
        entityType = iw.EntityType.RECTANGLE, 
        transform = transformRaw,
        color = colorRaw,
        zOrder = zOrderRaw,
        rectangle = iw.AddRectangleParams(halfExtents = halfExtentsRaw)
    )
    
    return sendCommandToApp(iw.Command.AddEntity, rectangleParams, iw.AddEntityReply).toMCPPayload()

@mcp.tool()
def addTriangleEntity(
    transform: Annotated[iw.TransformMCPPayload, Field(description = f"specifies how to place this new triangle. {iw.TRANSFORM_DOC}")],
    color: Annotated[iw.ColorMCPPayload, Field(description = f"specifies the color to draw this shape. {iw.COLOR_DOC}")],
    zOrder: Annotated[int, Field(description = f"the Z order {iw.Z_ORDER_DOC}")],
    point1: Annotated[iw.Vec2MCPPayload, Field(description = f"position of first point. {iw.VEC2_DOC}")],
    point2: Annotated[iw.Vec2MCPPayload, Field(description = f"position of second point. {iw.VEC2_DOC}")],
    point3: Annotated[iw.Vec2MCPPayload, Field(description = f"position of third point. {iw.VEC2_DOC}")],
) -> iw.AddEntityReplyMCPPayload:
    """
    Places a new triangle within the world at specified transform and color
    Note that each point specifies a position in the shape's local space, whose world space is then computed
    via transformation by the transform parameter
    """

    # create addTriangle command params
    transformRaw = iw.Transform()
    transformRaw.fromMCPPayload(transform)

    colorRaw = iw.Color()
    colorRaw.fromMCPPayload(color)

    zOrderRaw = ctypes.c_int(zOrder)

    point1Raw = iw.Vec2()
    point1Raw.fromMCPPayload(point1)

    point2Raw = iw.Vec2()
    point2Raw.fromMCPPayload(point2)

    point3Raw = iw.Vec2()
    point3Raw.fromMCPPayload(point3)

    triangleParams = iw.AddEntityParams(
        entityType = iw.EntityType.TRIANGLE, 
        transform = transformRaw,
        color = colorRaw,
        zOrder = zOrderRaw,
        triangle = iw.AddTriangleParams(
            point1 = point1Raw,
            point2 = point2Raw,
            point3 = point3Raw,
        )
    )

    return sendCommandToApp(iw.Command.AddEntity, triangleParams, iw.AddEntityReply).toMCPPayload()


@mcp.tool()
def removeEntity(
    entityId: Annotated[int, Field(description = "The id of the entity to remove")],
) -> iw.PlainReplyMCPPayload:
    """
    Allows entity removal given its id
    """

    entityIdRaw = ctypes.c_uint64(entityId)
    return sendCommandToApp(iw.Command.RemoveEntity, entityIdRaw, iw.PlainReply).toMCPPayload()

if __name__ == "__main__":

    mcp.run()
