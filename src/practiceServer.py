"""ImageWriter MCP server.

A stdio-based Model Context Protocol server. The Claude host app spawns this
process on startup and communicates with it over stdin/stdout, so nothing here
should print to stdout directly (that stream is the protocol channel). Use the
`logging` module, which writes to stderr, for any diagnostics.
"""

import tcpCommon, socket
from tcpCommon import *

import logging
import ctypes
from ctypes import *

from mcp.server.fastmcp import FastMCP

# Logs go to stderr; stdout is reserved for the MCP protocol.
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger("imagewriter")

# TODOs
# rename this file to ImageWriterMCP.py
# rename tcpServer.py and tcpClient.py to something more meaningful
#   - server is being used in production, client is just for testing
# let user specify image name during export
# Rename DummyReply, should mainly contain status


mcp = FastMCP(
    "ImageWriter",
    instructions=(
        "This server generates images of any given size and exports them to disk"
        "In all of the Draw* commands, a centerX value of 0 is the far left and increases as you move to the right"
        "Likewise a centerY value of 0 is the top of the image and increases as you move down"
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
def setupImage(width: int, height: int) -> bool:
    """Sets up a blank image canvas for drawing things to

    Image starts off as black and will get contents written to it as
    various Draw* commands are issued.
    Returns True on success, False on failure
    """
    if not connectToImageWriterApp():
        return False

    commIn = Command.SetupImage
    commandBuffer = bytes(commIn.value.to_bytes(COMMAND_SIZE))

    # create setupImage command params
    setupImageParams = SetupImageParams(width = width, height = height)
    logger.info(f"To server: {setupImageParams.toString()}")

    # serialize params
    paramsBuffer = bytes(setupImageParams)

    # send message to server
    try:
        logger.info(f"setupImage: sending command buffer")
        connToServer.sendall(commandBuffer)
        logger.info(f"setupImage: sending param buffer")
        connToServer.sendall(paramsBuffer)

    # will fail if server had disconnected at time of sending message
    # return to connecting state
    except ConnectionResetError as e:
        logger.error(f"ConnectionResetError: {e}\n\n")
        exit(1) # todo: kick off reconnect

    replyBuffer = bytearray()
    
    # wait here and receive message from client
    status, errorMessage = receiveMessage(buffer = replyBuffer, connection = connToServer, size = ctypes.sizeof(DummyReply))
    if not status:
        # log error message and return to connecting state
        logger.error(errorMessage)
        exit(1) # todo: kick off reconnect
    
    # deserialize client message and log
    reply = DummyReply.from_buffer_copy(replyBuffer)
    logger.info(f"From server: {reply.toString()}")

    disconnectFromImageWriter()
    return reply.status

@mcp.tool()
def drawCircle(centerX: int, centerY: int, radius: int) -> bool:
    """draws a circle on an image
    Importantly, this should not be called before an image is set up or it will fail
    Returns True on success, False on failure
    """

    if not connectToImageWriterApp():
        return False

    commIn = Command.DrawCircle
    commandBuffer = bytes(commIn.value.to_bytes(COMMAND_SIZE))

    # create drawCircle command params
    circleParams = DrawCircleParams(centerX = centerX, centerY = centerY, radius = radius)
    logger.info(f"To server: {circleParams.toString()}")

    # serialize params
    paramsBuffer = bytes(circleParams)

    # send message to server
    try:
        connToServer.sendall(commandBuffer)
        connToServer.sendall(paramsBuffer)

    # will fail if server had disconnected at time of sending message
    # return to connecting state
    except ConnectionResetError as e:
        logger.error(f"ConnectionResetError: {e}\n\n")
        exit(1) # todo: kick off reconnect

    replyBuffer = bytearray()
    
    # wait here and receive message from client
    status, errorMessage = receiveMessage(buffer = replyBuffer, connection = connToServer, size = ctypes.sizeof(DummyReply))
    if not status:
        # log error message and return to connecting state
        logger.error(errorMessage)
        exit(1) # todo: kick off reconnect
    
    # deserialize client message and log
    reply = DummyReply.from_buffer_copy(replyBuffer)
    logger.info(f"From server: {reply.toString()}")

    # serialize params
    paramsBuffer = bytes(circleParams)


    disconnectFromImageWriter()

    return reply.status

@mcp.tool()
def drawRectangle(centerX: int, centerY: int, halfExtentX: int, halfExtentY: int) -> bool:
    """draws a rectangle on an image

    Importantly, this should not be called before an image is set up or it will fail
    Returns True on success, False on failure
    """

    if not connectToImageWriterApp():
        return False

    commIn = Command.DrawRectangle
    commandBuffer = bytes(commIn.value.to_bytes(COMMAND_SIZE))

    # create drawRectangle command params
    rectangleParams = DrawRectangleParams(centerX = centerX, centerY = centerY, halfExtentX = halfExtentX, halfExtentY = halfExtentY)
    logger.info(f"To server: {rectangleParams.toString()}")

    # serialize params
    paramsBuffer = bytes(rectangleParams)
    # send message to server
    try:
        connToServer.sendall(commandBuffer)
        connToServer.sendall(paramsBuffer)

    # will fail if server had disconnected at time of sending message
    # return to connecting state
    except ConnectionResetError as e:
        logger.error(f"ConnectionResetError: {e}\n\n")
        exit(1) # todo: kick off reconnect

    replyBuffer = bytearray()
    
    # wait here and receive message from client
    status, errorMessage = receiveMessage(buffer = replyBuffer, connection = connToServer, size = ctypes.sizeof(DummyReply))
    if not status:
        # log error message and return to connecting state
        logger.error(errorMessage)
        exit(1) # todo: kick off reconnect
    
    # deserialize client message and log
    reply = DummyReply.from_buffer_copy(replyBuffer)
    logger.info(f"From server: {reply.toString()}")

    # serialize params
    paramsBuffer = bytes(rectangleParams)

    disconnectFromImageWriter()

    return reply.status


@mcp.tool()
def exportImage() -> bool:
    """exports the image to disk

    Importantly, this should not be called before an image is set up or it will fail
    Returns True on success, False on failure
    """

    if not connectToImageWriterApp():
        return False
    
    commIn = Command.ExportImage
    commandBuffer = bytes(commIn.value.to_bytes(COMMAND_SIZE))

    # create exportImage command params
    imageName = "SomeDumbImage"
    exportImageParams = ExportImageParams(imageName = imageName.encode('utf-8'))
    logger.info(f"To server: {exportImageParams.toString()}")

    # serialize params
    paramsBuffer = bytes(exportImageParams)

    # send message to server
    try:
        connToServer.sendall(commandBuffer)
        connToServer.sendall(paramsBuffer)

    # will fail if server had disconnected at time of sending message
    # return to connecting state
    except ConnectionResetError as e:
        logger.error(f"ConnectionResetError: {e}\n\n")
        exit(1) # todo: kick off reconnect

    replyBuffer = bytearray()
    
    # wait here and receive message from client
    status, errorMessage = receiveMessage(buffer = replyBuffer, connection = connToServer, size = ctypes.sizeof(DummyReply))
    if not status:
        # log error message and return to connecting state
        logger.error(errorMessage)
        exit(1) # todo: kick off reconnect
    
    # deserialize client message and log
    reply = DummyReply.from_buffer_copy(replyBuffer)
    logger.info(f"From server: {reply.toString()}")

    disconnectFromImageWriter()
    return reply.status


if __name__ == "__main__":

    mcp.run()
