import ctypes, socket
from pydantic import BaseModel
from enum import IntEnum
from ImageWriterCommon import *

HOST_IP = '127.0.0.1'
PORT = 65432
COMMAND_SIZE = 1

# Commands
class Command(IntEnum):

    SetCameraTransform = 1
    GetCameraTransform = 2
    RemoveAllEntities = 3
    AddEllipse = 4
    AddRectangle = 5
    AddTriangle = 6
    Disconnecting = 7

# Parameters
class SetCameraTransformParams(ctypes.Structure):
    _fields_ = [
        ("transform", Transform)
    ]

    def __str__(self):
        return f"(AddEllipseParams: transform = {self.transform})"


class AddEllipseParams(ctypes.Structure):
    _fields_ = [
        ("transform", Transform),
        ("color", Color),
        ("halfExtents", Vec2)
    ]

    def __str__(self):
        return f"(AddEllipseParams: transform = {self.transform}, color = {self.color}, halfExtents = {self.halfExtents})"

class AddRectangleParams(ctypes.Structure):
    _fields_ = [
        ("transform", Transform),
        ("color", Color),
        ("halfExtents", Vec2)
    ]

    def __str__(self):
        return f"(AddRectangleParams: transform = {self.transform}, color = {self.color}, halfExtents = {self.halfExtents})"

class AddTriangleParams(ctypes.Structure):
    _fields_ = [
        ("transform", Transform),
        ("color", Color),
        ("point1", Vec2),
        ("point2", Vec2),
        ("point3", Vec2)
    ]

    def __str__(self):
        return f"(AddTriangleParams: transform = {self.transform}, color = {self.color}, point1 = {self.point1}, point2 = {self.point2}, point3 = {self.point3})"

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

class TransformReply(ctypes.Structure):
    type MCPPayload = TransformReplyMCPPayload
    _fields_ = [
        ("status", Status),
        ("transform", Transform),
    ]

    def __str__(self):
        return f"(TransformReply: status = {self.status}, transform = {self.transform})"

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