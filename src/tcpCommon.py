import ctypes
import socket
from enum import IntEnum

HOST_IP = '127.0.0.1'
PORT = 65432
MAX_IMAGE_FILENAME_LENGTH = 64
COMMAND_SIZE = 1

class Command(IntEnum):

    SetupImage = 1
    DrawCircle = 2
    DrawRectangle = 3
    ExportImage = 4
    Disconnecting = 5

# Message to server
class SetupImageParams(ctypes.Structure):
    _fields_ = [
        ("width", ctypes.c_int),
        ("height", ctypes.c_int),
    ]

    def toString(self):
        return f"SetupImage params: width = {self.width}, height = {self.height}"
    
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

class ExportImageParams(ctypes.Structure):
    _fields_ = [
        ("imageName", ctypes.c_char * MAX_IMAGE_FILENAME_LENGTH),
    ]

    def toString(self):
        return f"ExportImage params: imageName = {self.imageName.decode('utf-8')}"


# Message to client
class DummyReply(ctypes.Structure):
    _fields_ = [
        ("status", ctypes.c_bool),
        ("length", ctypes.c_float),
        ("width", ctypes.c_float),
        ("height", ctypes.c_float),
        ("resources", ctypes.c_int),
    ]

    def toString(self):
        return f"DummyReply: status = {self.status}, length = {self.length}, width = {self.width}, height = {self.height}, resources = {self.resources}"


def receiveMessage(buffer: bytearray, connection: socket.socket, size: int) -> tuple[bool, str]:

    # read server message from client
    try:
        while len(buffer) < size:
            fragmentReceived = connection.recv(size - len(buffer))

            # error check on data received
            if not fragmentReceived:
                # sanity check buffer
                return (False, "receiveMessage: failed to receive data from client")
            
            buffer.extend(fragmentReceived)

    # will fail if client disconnects without gracefully telling us
    # return to listening state
    except (ConnectionResetError, ConnectionAbortedError) as e:
        return (False, f"receiveMessage: {e}")

    return (True, "")