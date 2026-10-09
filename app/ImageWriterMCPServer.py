"""ImageWriter MCP server.

A stdio-based Model Context Protocol server. The Claude host app spawns this
process on startup and communicates with it over stdin/stdout, so nothing here
should print to stdout directly (that stream is the protocol channel). Use the
`logging` module, which writes to stderr, for any diagnostics.
"""

import socket
from ImageWriterProtocol import *

import logging
import ctypes
from ctypes import *
from typing import Annotated

from mcp.server.mcpserver import MCPServer
from mcp.server.mcpserver.exceptions import ToolError

# Logs go to stderr; stdout is reserved for the MCP protocol.
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger("imagewriter")


mcp = MCPServer(
    "ImageWriter",
    instructions=(
        "This server connects to an image writer application tells it to draw whatever the user wishes. "
        "With every shape you draw, you specify a Transform describing its position, orientation and scale "
        "in the 2D world, in addition you can also query the camera Transform to see what the user is currently looking at"
    ),
)

def connectToImageWriterApp() -> bool:

    global connToServer
    
    try:
        connToServer = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        connToServer.connect((HOST_IP, PORT))
        logger.info(f"Establised connection to image writer app!")
    except (TimeoutError, ConnectionRefusedError) as e:
        logger.info(f"Connection attempt failed: {e}\nTrying again...")
        connToServer.close()
        return False
    
    return True

def disconnectFromImageWriter():

    commIn = Command.Disconnecting
    commandBuffer = bytes(commIn.value.to_bytes(COMMAND_SIZE))

    try:
        connToServer.sendall(commandBuffer)
    except ConnectionResetError as e:
        logger.error(f"Note: disconnecting failed: ConnectionResetError: {e}\n\n")
    finally:
        connToServer.close()

@mcp.tool()
def setCameraTransform(
    transform: Annotated[TransformMCPPayload, Field(description = f"specifies the new transform of the camera. {TRANSFORM_DOC}")],
) -> PlainReplyMCPPayload:
    """sets the transform of the camera, note the scale determines the camera view rectangle's half Y extent
    where the halfX extent is then internally determined by the screen aspect ratio
    """

    if not connectToImageWriterApp():
        raise ToolError("setCameraTransform(): Failed to connect to ImageWriter app")

    commIn = Command.SetCameraTransform
    commandBuffer = bytes(commIn.value.to_bytes(COMMAND_SIZE))

    transformRaw = Transform()
    transformRaw.fromMCPPayload(transform)

    params = SetCameraTransformParams(transform = transformRaw)
    logger.info(f"To server: {params}")

    # serialize params
    paramsBuffer = bytes(params)

    # send message to server
    try:
        connToServer.sendall(commandBuffer)
        connToServer.sendall(paramsBuffer)

    # will fail if server had disconnected at time of sending message
    except ConnectionResetError as e:
        logger.error(f"ConnectionResetError: {e}\n\n")
        disconnectFromImageWriter()
        raise ToolError("setCameraTransform(): Failed to send command to ImageWriter app")

    replyBuffer = bytearray()
    
    # wait here and receive message from client
    success, errorMessage = receiveMessage(buffer = replyBuffer, connection = connToServer, size = ctypes.sizeof(PlainReply))
    if not success:
        # log error message and return to connecting state
        logger.error(errorMessage)
        disconnectFromImageWriter()
        raise ToolError("setCameraTransform(): Failed to receive reply from ImageWriter app")
    
    # deserialize client message and log
    reply = PlainReply.from_buffer_copy(replyBuffer)
    logger.info(f"From server: {reply}")

    disconnectFromImageWriter()

    return reply.toMCPPayload()

@mcp.tool()
def getCameraTransform() -> CameraTransformReplyMCPPayload:
    """returns the rectangle representing the camera's world transform. Note that the "scale" parameter denotes the
    half Y extent of the camera in world units where the half X extent can be obtained from multiplying that by the 
    returned widthFromHeight value
  
    """

    if not connectToImageWriterApp():
        raise ToolError("getCameraTransform(): Failed to connect to ImageWriter app")

    commIn = Command.GetCameraTransform
    commandBuffer = bytes(commIn.value.to_bytes(COMMAND_SIZE))

    # send command to server
    try:
        connToServer.sendall(commandBuffer)

    # will fail if server had disconnected at time of sending message
    except ConnectionResetError as e:
        logger.error(f"ConnectionResetError: {e}\n\n")
        disconnectFromImageWriter()
        raise ToolError("getCameraTransform(): Failed to send command to ImageWriter app")

    replyBuffer = bytearray()
    
    # wait here and receive message from client
    success, errorMessage = receiveMessage(buffer = replyBuffer, connection = connToServer, size = ctypes.sizeof(TransformReply))
    if not success:
        # log error message and return to connecting state
        logger.error(errorMessage)
        disconnectFromImageWriter()
        raise ToolError("getCameraTransform(): Failed to receive reply from ImageWriter app")
    
    # deserialize client message and log
    reply = TransformReply.from_buffer_copy(replyBuffer)
    logger.info(f"From server: {reply}")

    disconnectFromImageWriter()

    return reply.toCameraMCPPayload()

@mcp.tool()
def removeAllEntities() -> PlainReplyMCPPayload:
    """clears the world of all spawned entities"""

    if not connectToImageWriterApp():
        raise ToolError("removeAllEntities(): Failed to connect to ImageWriter app")

    commIn = Command.RemoveAllEntities
    commandBuffer = bytes(commIn.value.to_bytes(COMMAND_SIZE))

    # send message to server
    try:
        connToServer.sendall(commandBuffer)

    # will fail if server had disconnected at time of sending message
    except ConnectionResetError as e:
        logger.error(f"ConnectionResetError: {e}\n\n")
        disconnectFromImageWriter()
        raise ToolError("removeAllEntities(): Failed to send command to ImageWriter app")

    replyBuffer = bytearray()
    
    # wait here and receive message from client
    success, errorMessage = receiveMessage(buffer = replyBuffer, connection = connToServer, size = ctypes.sizeof(PlainReply))
    if not success:
        # log error message and return to connecting state
        logger.error(errorMessage)
        disconnectFromImageWriter()
        raise ToolError("removeAllEntities(): Failed to receive reply from ImageWriter app")
    
    # deserialize client message and log
    reply = PlainReply.from_buffer_copy(replyBuffer)
    logger.info(f"From server: {reply}")

    disconnectFromImageWriter()

    return reply.toMCPPayload()

@mcp.tool()
def addEllipse(
        transform: Annotated[TransformMCPPayload, Field(description = f"specifies how to place this new ellipse. {TRANSFORM_DOC}")],
        color: Annotated[ColorMCPPayload, Field(description = f"specifies the color to draw this shape. {COLOR_DOC}")],
        halfExtents: Annotated[Vec2MCPPayload, Field(description = f"half extents of the ellipse in its local space. {VEC2_DOC}")],
) -> PlainReplyMCPPayload:
    """
    Places a new ellipse within the world at specified transform and color
    Note that the half extents define the shape's size in its own local space, where its global size is then obtained by multiplying
    the extents by the shape's scale transform
    """

    if not connectToImageWriterApp():
        raise ToolError("addEllipse(): Failed to connect to ImageWriter app")

    commIn = Command.AddEllipse
    commandBuffer = bytes(commIn.value.to_bytes(COMMAND_SIZE))

    # create addEllipse command params
    transformRaw = Transform()
    transformRaw.fromMCPPayload(transform)

    colorRaw = Color()
    colorRaw.fromMCPPayload(color)

    halfExtentsRaw = Vec2()
    halfExtentsRaw.fromMCPPayload(halfExtents)

    ellipseParams = AddEllipseParams(transform = transformRaw, color = colorRaw, halfExtents = halfExtentsRaw)
    logger.info(f"To server: {ellipseParams}")

    # serialize params
    paramsBuffer = bytes(ellipseParams)

    # send message to server
    try:
        connToServer.sendall(commandBuffer)
        connToServer.sendall(paramsBuffer)

    # will fail if server had disconnected at time of sending message
    except ConnectionResetError as e:
        logger.error(f"ConnectionResetError: {e}\n\n")
        disconnectFromImageWriter()
        raise ToolError("addEllipse(): Failed to send command to ImageWriter app")

    replyBuffer = bytearray()
    
    # wait here and receive message from client
    success, errorMessage = receiveMessage(buffer = replyBuffer, connection = connToServer, size = ctypes.sizeof(PlainReply))
    if not success:
        # log error message and return to connecting state
        logger.error(errorMessage)
        disconnectFromImageWriter()
        raise ToolError("addEllipse(): Failed to receive reply from ImageWriter app")
    
    # deserialize client message and log
    reply = PlainReply.from_buffer_copy(replyBuffer)
    logger.info(f"From server: {reply}")

    disconnectFromImageWriter()

    return reply.toMCPPayload()
    
@mcp.tool()
def addRectangle(
    transform: Annotated[TransformMCPPayload, Field(description = f"specifies how to place this new rectangle. {TRANSFORM_DOC}")],
    color: Annotated[ColorMCPPayload, Field(description = f"specifies the color to draw this shape. {COLOR_DOC}")],
    halfExtents: Annotated[Vec2MCPPayload, Field(description = f"half extents of the rectangle in its local space. {VEC2_DOC}")],
) -> PlainReplyMCPPayload:
    """
    Places a new rectangle within the world at specified transform and color
    Note that the half extents define the shape's size in its own local space, where its global size is then obtained by multiplying
    the extents by the shape's scale transform
    """

    if not connectToImageWriterApp():
        raise ToolError("addRectangle(): Failed to connect to ImageWriter app")

    commIn = Command.AddRectangle
    commandBuffer = bytes(commIn.value.to_bytes(COMMAND_SIZE))

    # create addRectangle command params
    transformRaw = Transform()
    transformRaw.fromMCPPayload(transform)

    colorRaw = Color()
    colorRaw.fromMCPPayload(color)

    halfExtentsRaw = Vec2()
    halfExtentsRaw.fromMCPPayload(halfExtents)

    rectangleParams = AddRectangleParams(transform = transformRaw, color = colorRaw, halfExtents = halfExtentsRaw)
    logger.info(f"To server: {rectangleParams}")

    # serialize params
    paramsBuffer = bytes(rectangleParams)
    # send message to server
    try:
        connToServer.sendall(commandBuffer)
        connToServer.sendall(paramsBuffer)

    # will fail if server had disconnected at time of sending message
    except ConnectionResetError as e:
        logger.error(f"ConnectionResetError: {e}\n\n")
        disconnectFromImageWriter()
        raise ToolError("addRectangle(): Failed to send command to ImageWriter app")

    replyBuffer = bytearray()
    
    # wait here and receive message from client
    success, errorMessage = receiveMessage(buffer = replyBuffer, connection = connToServer, size = ctypes.sizeof(PlainReply))
    if not success:
        # log error message and return to connecting state
        logger.error(errorMessage)
        disconnectFromImageWriter()
        raise ToolError("addRectangle(): Failed to receive reply from ImageWriter app")
    
    # deserialize client message and log
    reply = PlainReply.from_buffer_copy(replyBuffer)
    logger.info(f"From server: {reply}")

    disconnectFromImageWriter()

    return reply.toMCPPayload()

@mcp.tool()
def addTriangle(
    transform: Annotated[TransformMCPPayload, Field(description = f"specifies how to place this new triangle. {TRANSFORM_DOC}")],
    color: Annotated[ColorMCPPayload, Field(description = f"specifies the color to draw this shape. {COLOR_DOC}")],
    point1: Annotated[Vec2MCPPayload, Field(description = f"position of first point. {VEC2_DOC}")],
    point2: Annotated[Vec2MCPPayload, Field(description = f"position of second point. {VEC2_DOC}")],
    point3: Annotated[Vec2MCPPayload, Field(description = f"position of third point. {VEC2_DOC}")],
) -> PlainReplyMCPPayload:
    """
    Places a new triangle within the world at specified transform and color
    Note that each point specifies a position in the shape's local space, whose world space is then computed
    via transformation by the transform parameter
    """

    if not connectToImageWriterApp():
        raise ToolError("addTriangle(): Failed to connect to ImageWriter app")

    commIn = Command.AddTriangle
    commandBuffer = bytes(commIn.value.to_bytes(COMMAND_SIZE))

    # create addTriangle command params
    transformRaw = Transform()
    transformRaw.fromMCPPayload(transform)

    colorRaw = Color()
    colorRaw.fromMCPPayload(color)

    point1Raw = Vec2()
    point1Raw.fromMCPPayload(point1)

    point2Raw = Vec2()
    point2Raw.fromMCPPayload(point2)

    point3Raw = Vec2()
    point3Raw.fromMCPPayload(point3)

    triangleParams = AddTriangleParams(
        transform = transformRaw, 
        color = colorRaw,
        point1 = point1Raw,
        point2 = point2Raw,
        point3 = point3Raw
    )
    logger.info(f"To server: {triangleParams}")

    # serialize params
    paramsBuffer = bytes(triangleParams)
    # send message to server
    try:
        connToServer.sendall(commandBuffer)
        connToServer.sendall(paramsBuffer)

    # will fail if server had disconnected at time of sending message
    except ConnectionResetError as e:
        logger.error(f"ConnectionResetError: {e}\n\n")
        disconnectFromImageWriter()
        raise ToolError("addTriangle(): Failed to send command to ImageWriter app")

    replyBuffer = bytearray()
    
    # wait here and receive message from client
    success, errorMessage = receiveMessage(buffer = replyBuffer, connection = connToServer, size = ctypes.sizeof(PlainReply))
    if not success:
        # log error message and return to connecting state
        logger.error(errorMessage)
        disconnectFromImageWriter()
        raise ToolError("addTriangle(): Failed to receive reply from ImageWriter app")
    
    # deserialize client message and log
    reply = PlainReply.from_buffer_copy(replyBuffer)
    logger.info(f"From server: {reply}")

    disconnectFromImageWriter()

    return reply.toMCPPayload()

@mcp.tool()
def testAddVec2(
    point: Annotated[Vec2MCPPayload, Field(description = f"A vec2 value to test and print. {VEC2_DOC}")]
):
    """
    Tests taking in a vec2 and prints them to make sure they have correct values
    """
    pointRaw = Vec2()
    pointRaw.fromMCPPayload(point)
    logger.info(f"Received vec2 value: {pointRaw}")

if __name__ == "__main__":

    mcp.run()
