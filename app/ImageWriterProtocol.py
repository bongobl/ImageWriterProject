import ctypes, socket
from pydantic import BaseModel
from enum import IntEnum

HOST_IP = '127.0.0.1'
PORT = 65432
MAX_IMAGE_FILENAME_LENGTH = 64
MAX_REPLY_MESSAGE_LENGTH = 256
COMMAND_SIZE = 1

class Command(IntEnum):

    GetCameraView = 1
    ClearImage = 2
    DrawCircle = 3
    DrawRectangle = 4
    Disconnecting = 5

class DrawCircleParams(ctypes.Structure):
    _fields_ = [
        ("centerX", ctypes.c_float),
        ("centerY", ctypes.c_float),
        ("radius", ctypes.c_float),
    ]

    def toString(self):
        return f"Circle params: centerX = {self.centerX}, centerY = {self.centerY}, radius = {self.radius}"

class DrawRectangleParams(ctypes.Structure):
    _fields_ = [
        ("centerX", ctypes.c_float),
        ("centerY", ctypes.c_float),
        ("halfExtentX", ctypes.c_float),
        ("halfExtentY", ctypes.c_float),
    ]

    def toString(self):
        return f"Rectange params: centerX = {self.centerX}, centerY = {self.centerY}, halfExtentX = {self.halfExtentX}, halfExtentY = {self.halfExtentY}"

class PlainStatusReplyMCPPayload(BaseModel):
    success: bool = False
    message: str = ""

# Server reply to client after each command
class PlainStatusReply(ctypes.Structure):
    type MCPPayload = PlainStatusReplyMCPPayload
    _fields_ = [
        ("success", ctypes.c_bool),
        ("message", ctypes.c_char * MAX_REPLY_MESSAGE_LENGTH),
    ]

    def toString(self):
        return f"PlainStatusReply: success = {self.success}, message = {self.message.decode('utf-8')}"

    def toMCPPayload(self) -> MCPPayload:
        return PlainStatusReplyMCPPayload(success = self.success, message = self.message.decode('utf-8'))



class CameraViewReplyMCPPayload(BaseModel):
    success: bool = False
    message: str = ""
    posX: float = 0
    posY: float = 0
    maxX: float = 0
    maxY: float = 0
    angle: float = 0

class CameraViewReply(ctypes.Structure):
    type MCPPayload = CameraViewReplyMCPPayload
    _fields_ = [
        ("success", ctypes.c_bool),
        ("message", ctypes.c_char * MAX_REPLY_MESSAGE_LENGTH),
        ("posX", ctypes.c_float),
        ("posY", ctypes.c_float),
        ("maxX", ctypes.c_float),
        ("maxY", ctypes.c_float),
        ("angle", ctypes.c_float)
    ]

    def toString(self):
        return f"CameraViewReply: success = {self.success}, message = {self.message.decode('utf-8')}, posX = {self.posX}, posY = {self.posY}, maxX = {self.maxX}, maxY = {self.maxY}, angle = {self.angle}"

    def toMCPPayload(self) -> MCPPayload:
        return CameraViewReplyMCPPayload(success = self.success, message = self.message.decode('utf-8'), posX = self.posX, posY = self.posY, maxX = self.maxX, maxY = self.maxY, angle = self.angle)

def receiveMessage(buffer: bytearray, connection: socket.socket, size: int) -> tuple[bool, str]:

    # read server message from client
    try:
        while len(buffer) < size:
            fragmentReceived = connection.recv(size - len(buffer))

            # error check on data received
            if not fragmentReceived:
                # sanity check buffer
                return (False, "receiveMessage: failed to receive data")
            
            buffer.extend(fragmentReceived)

    # will fail if client disconnects without gracefully telling us
    # return to listening state
    except (ConnectionResetError, ConnectionAbortedError) as e:
        return (False, f"receiveMessage: {e}")

    return (True, "")