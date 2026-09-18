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
def drawCircle(positionX: float, positionY: float, radius: float) -> PlainReply.MCPPayload:
    """places a new circle within the world at specified coordinates and size"""

    if not connectToImageWriterApp():
        raise ToolError("drawCircle(): Failed to connect to ImageWriter app")

    commIn = Command.DrawCircle
    commandBuffer = bytes(commIn.value.to_bytes(COMMAND_SIZE))

    # create drawCircle command params
    circleParams = DrawCircleParams(centerX = positionX, centerY = positionY, radius = radius)
    logger.info(f"To server: {circleParams.toString()}")

    # serialize params
    paramsBuffer = bytes(circleParams)

    # send message to server
    try:
        connToServer.sendall(commandBuffer)
        connToServer.sendall(paramsBuffer)

    # will fail if server had disconnected at time of sending message
    except ConnectionResetError as e:
        logger.error(f"ConnectionResetError: {e}\n\n")
        disconnectFromImageWriter()
        raise ToolError("drawCircle(): Failed to send command to ImageWriter app")

    replyBuffer = bytearray()
    
    # wait here and receive message from client
    success, errorMessage = receiveMessage(buffer = replyBuffer, connection = connToServer, size = ctypes.sizeof(PlainReply))
    if not success:
        # log error message and return to connecting state
        logger.error(errorMessage)
        disconnectFromImageWriter()
        raise ToolError("drawCircle(): Failed to receive reply from ImageWriter app")
    
    # deserialize client message and log
    reply = PlainReply.from_buffer_copy(replyBuffer)
    logger.info(f"From server: {reply.toString()}")

    disconnectFromImageWriter()

    return reply.toMCPPayload()

@mcp.tool()
def drawRectangle(positionX: float, positionY: float, halfExtentX: float, halfExtentY: float) -> PlainReply.MCPPayload:
    """places a new rectangle within the world at specified coordinates and size"""

    if not connectToImageWriterApp():
        raise ToolError("drawRectangle(): Failed to connect to ImageWriter app")

    commIn = Command.DrawRectangle
    commandBuffer = bytes(commIn.value.to_bytes(COMMAND_SIZE))

    # create drawRectangle command params
    rectangleParams = DrawRectangleParams(centerX = positionX, centerY = positionY, halfExtentX = halfExtentX, halfExtentY = halfExtentY)
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
        raise ToolError("drawRectangle(): Failed to send command to ImageWriter app")

    replyBuffer = bytearray()
    
    # wait here and receive message from client
    success, errorMessage = receiveMessage(buffer = replyBuffer, connection = connToServer, size = ctypes.sizeof(PlainReply))
    if not success:
        # log error message and return to connecting state
        logger.error(errorMessage)
        disconnectFromImageWriter()
        raise ToolError("drawRectangle(): Failed to receive reply from ImageWriter app")
    
    # deserialize client message and log
    reply = PlainReply.from_buffer_copy(replyBuffer)
    logger.info(f"From server: {reply.toString()}")

    disconnectFromImageWriter()

    return reply.toMCPPayload()


if __name__ == "__main__":

    mcp.run()
