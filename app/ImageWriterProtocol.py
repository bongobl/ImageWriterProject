import ctypes, socket
from pydantic import BaseModel
from enum import IntEnum
from ImageWriterCommon import *

HOST_IP = '127.0.0.1'
PORT = 65432
MAX_IMAGE_FILENAME_LENGTH = 64
COMMAND_SIZE = 1

# Commands
class Command(IntEnum):

    GetCameraTransform = 1
    ClearImage = 2
    DrawCircle = 3
    DrawRectangle = 4
    Disconnecting = 5

# Parameters
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

# Replies

class PlainReplyMCPPayload(BaseModel):
    status: StatusMCPPayload = ()

# Server reply to client after each command
class PlainReply(ctypes.Structure):
    type MCPPayload = PlainReplyMCPPayload
    _fields_ = [
        ("status", Status)
    ]

    def toString(self):
        return f"(PlainReply: status = {self.status.toString()})"

    def toMCPPayload(self) -> MCPPayload:
        return PlainReplyMCPPayload(status = self.status.toMCPPayload())


class TransformReplyMCPPayload(BaseModel):
    status: StatusMCPPayload = ()
    transform: TransformMCPPayload = ()

class TransformReply(ctypes.Structure):
    type MCPPayload = TransformReplyMCPPayload
    _fields_ = [
        ("status", Status),
        ("transform", Transform),
    ]

    def toString(self):
        return f"(TransformReply: status = {self.status.toString()}, transform = {self.transform.toString()})"

    def toMCPPayload(self) -> MCPPayload:

        return TransformReplyMCPPayload(
            status = self.status.toMCPPayload(),
            transform = self.transform.toMCPPayload()
        )


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