import ctypes, socket
from pydantic import BaseModel
from enum import IntEnum
from .Common import *

HOST_IP = '127.0.0.1'
PORT = 65432
COMMAND_SIZE = 1

# Commands
class Command(IntEnum):

    SetCameraTransform = 1
    GetCameraTransform = 2
    RemoveAllEntities = 3
    AddEntity = 4
    Disconnecting = 5

# Parameters =================================================================================

# Empty for now

# Replies ====================================================================================

# Plain
class PlainReplyMCPPayload(BaseModel):
    success: bool = False
    message: str = ""

class PlainReply(ctypes.Structure):

    _fields_ = [
        ("success", ctypes.c_bool),
        ("message", ctypes.c_char * MAX_REPLY_MESSAGE_LENGTH),
    ]

    def __str__(self):
        return f"(PlainReply: success = {self.success}, message = {self.message.decode('utf-8')})"

    def toMCPPayload(self) -> PlainReplyMCPPayload:
        return PlainReplyMCPPayload(success = self.success, message = self.message.decode('utf-8'))

# Transform (additional MCPPayload type for Camera transform)
class TransformReplyMCPPayload(PlainReplyMCPPayload):
    transform: TransformMCPPayload

class CameraTransformReplyMCPPayload(TransformReplyMCPPayload):
    widthFromHeight: float = Field(..., description="(float), aspect ratio of the camera view, ie the width / height")

class TransformReply(PlainReply):

    _fields_ = [
        ("transform", Transform),
        ("cameraWidthFromHeight", ctypes.c_float), # only used when getting camera transform
    ]

    def __str__(self):
        return f"(TransformReply: success = {self.success}, message = {self.message.decode('utf-8')}, transform = {self.transform}, cameraWidthFromHeight = {self.cameraWidthFromHeight})"

    def toMCPPayload(self) -> TransformReplyMCPPayload:

        return TransformReplyMCPPayload(
            success = self.success, 
            message = self.message.decode('utf-8'),
            transform = self.transform.toMCPPayload()
        )

    def toCameraMCPPayload(self) -> CameraTransformReplyMCPPayload:

        return CameraTransformReplyMCPPayload(
            success = self.success, 
            message = self.message.decode('utf-8'),
            transform = self.transform.toMCPPayload(),
            widthFromHeight = self.cameraWidthFromHeight
        )

# ============================================================================================

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