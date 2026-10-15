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

# Parameters
class SetCameraTransformParams(ctypes.Structure):
    _fields_ = [
        ("transform", Transform)
    ]

    def __str__(self):
        return f"(SetCameraTransformParams: transform = {self.transform})"

# Replies

class PlainReplyMCPPayload(BaseModel):
    status: StatusMCPPayload

# Server reply to client after each command
class PlainReply(ctypes.Structure):
    type MCPPayload = PlainReplyMCPPayload
    _fields_ = [
        ("status", Status)
    ]

    def __str__(self):
        return f"(PlainReply: status = {self.status})"

    def toMCPPayload(self) -> MCPPayload:
        return PlainReplyMCPPayload(status = self.status.toMCPPayload())


class TransformReplyMCPPayload(BaseModel):
    status: StatusMCPPayload
    transform: TransformMCPPayload

class CameraTransformReplyMCPPayload(TransformReplyMCPPayload):
    widthFromHeight: float = Field(..., description="(float), aspect ratio of the camera view, ie the width / height")

class TransformReply(ctypes.Structure):
    type MCPPayload = TransformReplyMCPPayload
    type CameraMCPPayload = CameraTransformReplyMCPPayload

    _fields_ = [
        ("status", Status),
        ("transform", Transform),
        ("cameraWidthFromHeight", ctypes.c_float), # only used when getting camera transform
    ]

    def __str__(self):
        return f"(TransformReply: status = {self.status}, transform = {self.transform}, cameraWidthFromHeight = {self.cameraWidthFromHeight})"

    def toMCPPayload(self) -> MCPPayload:

        return TransformReplyMCPPayload(
            status = self.status.toMCPPayload(),
            transform = self.transform.toMCPPayload()
        )

    def toCameraMCPPayload(self) -> CameraMCPPayload:

        return CameraTransformReplyMCPPayload(
            status = self.status.toMCPPayload(),
            transform = self.transform.toMCPPayload(),
            widthFromHeight = self.cameraWidthFromHeight
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