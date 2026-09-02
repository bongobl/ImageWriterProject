"""ImageWriter MCP server.

A stdio-based Model Context Protocol server. The Claude host app spawns this
process on startup and communicates with it over stdin/stdout, so nothing here
should print to stdout directly (that stream is the protocol channel). Use the
`logging` module, which writes to stderr, for any diagnostics.
"""

import logging
import ctypes
from ctypes import *

from mcp.server.fastmcp import FastMCP

# Logs go to stderr; stdout is reserved for the MCP protocol.
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger("imagewriter")

# TODO: Move chronology guarding to Core layer
setupCalled = False

mcp = FastMCP(
    "ImageWriter",
    instructions=(
        "This server generates images of any given size and exports them to disk"
        "In all of the Draw* commands, a centerX value of 0 is the far left and increases as you move to the right"
        "Likewise a centerY value of 0 is the top of the image and increases as you move down"
    ),
)


# TODO: Mirror API.h: move to common header
class HImageWriterInstance(ctypes.Structure):
    _fields_ = [
        ("pData", ctypes.c_void_p),
    ]

@mcp.tool()
def setupImage(width: int, height: int) -> bool:
    """Sets up a blank image canvas for drawing things to

    Image starts off as black and will get contents written to it as
    various Draw* commands are issued.
    Returns True on success, False on failure
    """

    global setupCalled

    framework.SetupImage(instance, width, height)

    setupCalled = True

    return True

@mcp.tool()
def drawCircle(centerX: int, centerY: int, radius: int) -> bool:
    """draws a circle on an image
    Importantly, this should not be called before an image is set up
    Returns True on success, False on failure
    """

    if setupCalled:
        framework.DrawCircle(instance, centerX, centerY, radius)

    
    return setupCalled

@mcp.tool()
def drawRectangle(centerX: int, centerY: int, halfExtentX: int, halfExtentY: int) -> bool:
    """draws a rectangle on an image

    Importantly, this should not be called before an image is set up
    Returns True on success, False on failure
    """

    if setupCalled:
        framework.DrawRectangle(instance, centerX, centerY, halfExtentX, halfExtentY)

    return setupCalled

@mcp.tool()
def exportImage() -> bool:
    """exports the image to disk

    Importantly, this should not be called before an image is set up
    Returns True on success, False on failure
    """

    if setupCalled:
        framework.ExportImage(instance)

    return setupCalled



if __name__ == "__main__":

    # load image writer library and functions
    framework = ctypes.CDLL("C:\\Dev\\Practice\\BasicClaude\\ImageWriter\\buildGNU\\ImageWriterAPI.dll")

    framework.CreateImageWriterInstance.argtypes = [ctypes.POINTER(HImageWriterInstance)]
    framework.CreateImageWriterInstance.restype = None

    framework.SetupImage.argtypes = [HImageWriterInstance, ctypes.c_int, ctypes.c_int]
    framework.SetupImage.restype = None

    framework.DrawCircle.argtypes = [HImageWriterInstance, ctypes.c_int, ctypes.c_int, ctypes.c_int]
    framework.DrawCircle.restype = None

    framework.DrawRectangle.argtypes = [HImageWriterInstance, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int]
    framework.DrawRectangle.restype = None

    framework.ExportImage.argtypes = [HImageWriterInstance]
    framework.ExportImage.restype = None

    framework.DestroyImageWriterInstance.argtypes = [ctypes.POINTER(HImageWriterInstance)]
    framework.DestroyImageWriterInstance.restype = None

    # create instance
    instance = HImageWriterInstance()
    framework.CreateImageWriterInstance(ctypes.byref(instance))

    # Runs the server over stdio, the transport the Claude host app uses.
    mcp.run()

    # todo, move to server "destructor"
    framework.DestroyImageWriterInstance(instance)

