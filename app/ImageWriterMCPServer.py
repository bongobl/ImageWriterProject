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
        "This server connects to an image writer application tells it to draw whatever the user wishes"
        "Note the coordinate system is such that (x,y) = (0,0) is the center of the image the"
        "x axis range is roughly -12.5 to 12.5 and the y axis range is roughly -6.5 to 6.5"
        "X values increase as you move toward the right of the image"
        "Y values increase as you move toward the top of the image"
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
def getCameraTransform() -> TransformReply.MCPPayload:
    """returns the rectangle representing the camera's transform within the world which consists of a position, rotation
    and scale (in that order of most globally to most locally applied)
    Note a scale value along an axis of x means it spans that axis from -x to x.
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
    logger.info(f"From server: {reply.toString()}")

    disconnectFromImageWriter()

    return reply.toMCPPayload()

@mcp.tool()
def clearImage() -> PlainReply.MCPPayload:
    """clears the world of all drawn shapes"""

    if not connectToImageWriterApp():
        raise ToolError("clearImage(): Failed to connect to ImageWriter app")

    commIn = Command.ClearImage
    commandBuffer = bytes(commIn.value.to_bytes(COMMAND_SIZE))

    # send message to server
    try:
        connToServer.sendall(commandBuffer)

    # will fail if server had disconnected at time of sending message
    except ConnectionResetError as e:
        logger.error(f"ConnectionResetError: {e}\n\n")
        disconnectFromImageWriter()
        raise ToolError("clearImage(): Failed to send command to ImageWriter app")

    replyBuffer = bytearray()
    
    # wait here and receive message from client
    success, errorMessage = receiveMessage(buffer = replyBuffer, connection = connToServer, size = ctypes.sizeof(PlainReply))
    if not success:
        # log error message and return to connecting state
        logger.error(errorMessage)
        disconnectFromImageWriter()
        raise ToolError("clearImage(): Failed to receive reply from ImageWriter app")
    
    # deserialize client message and log
    reply = PlainReply.from_buffer_copy(replyBuffer)
    logger.info(f"From server: {reply.toString()}")

    disconnectFromImageWriter()

    return reply.toMCPPayload()

@mcp.tool()
def testAddColor(
    color: Annotated[ColorMCPPayload, Field(description = f"A color value to test and print. {COLOR_DOC}")]
):
    """
    Tests taking in color values and prints them to make sure they have correct values
    """
    colorRaw = Color()
    colorRaw.fromMCPPayload(color)
    logger.info(f"Received color value: {colorRaw.toString()}")


@mcp.tool()
def addEllipse(
        transform: Annotated[TransformMCPPayload, Field(description = f"specifies how to place this new ellipse. {TRANSFORM_DOC}")],
        color: Annotated[ColorMCPPayload, Field(description = f"specifies the color to draw this shape. {COLOR_DOC}")]
) -> PlainReply.MCPPayload:
    """
    Places a new ellipse within the world at specified transform and color
    Note that a (scaleX, scaleY) value of (1,1) represents a unit circle of radius 1
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

    ellipseParams = AddEllipseParams(transform = transformRaw, color = colorRaw)
    logger.info(f"To server: {ellipseParams.toString()}")

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
    logger.info(f"From server: {reply.toString()}")

    disconnectFromImageWriter()

    return reply.toMCPPayload()
    
@mcp.tool()
def addRectangle(
    transform: Annotated[TransformMCPPayload, Field(description = f"specifies how to place this new rectangle. {TRANSFORM_DOC}")],
    color: Annotated[ColorMCPPayload, Field(description = f"specifies the color to draw this shape. {COLOR_DOC}")]
) -> PlainReply.MCPPayload:
    """
    Places a new rectangle within the world at specified transform and color
    Note that a (scaleX, scaleY) value of (1,1) represents a square of width and height both equal to 2
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

    rectangleParams = AddRectangleParams(transform = transformRaw, color = colorRaw)
    logger.info(f"To server: {rectangleParams.toString()}")

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
    logger.info(f"From server: {reply.toString()}")

    disconnectFromImageWriter()

    return reply.toMCPPayload()


if __name__ == "__main__":

    mcp.run()
