import ctypes, socket
from pydantic import BaseModel
from enum import IntEnum

HOST_IP = '127.0.0.1'
PORT = 65432
MAX_IMAGE_FILENAME_LENGTH = 64
MAX_REPLY_MESSAGE_LENGTH = 256
COMMAND_SIZE = 1

class Command(IntEnum):

    DrawCircle = 2
    DrawRectangle = 3
    Disconnecting = 5

class DrawCircleParams(ctypes.Structure):
    _fields_ = [
        ("centerX", ctypes.c_int),
        ("centerY", ctypes.c_int),
        ("radius", ctypes.c_int),
    ]

    def toString(self):
        return f"Circle params: centerX = {self.centerX}, centerY = {self.centerY}, radius = {self.radius}"

class DrawRectangleParams(ctypes.Structure):
    _fields_ = [
        ("centerX", ctypes.c_int),
        ("centerY", ctypes.c_int),
        ("halfExtentX", ctypes.c_int),
        ("halfExtentY", ctypes.c_int),
    ]

    def toString(self):
        return f"Rectange params: centerX = {self.centerX}, centerY = {self.centerY}, halfExtentX = {self.halfExtentX}, halfExtentY = {self.halfExtentY}"

class MCPOutcome(BaseModel):
    success: bool = False
    message: str = ""

# Server reply to client after each command
class Reply(ctypes.Structure):
    _fields_ = [
        ("status", ctypes.c_bool),
        ("message", ctypes.c_char * MAX_REPLY_MESSAGE_LENGTH),
    ]

    def toString(self):
        return f"Reply: status = {self.status}, message = {self.message.decode('utf-8')}"

    def toMCPOutcome(self) -> MCPOutcome:
        return MCPOutcome(success = self.status, message = self.message.decode('utf-8'))



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