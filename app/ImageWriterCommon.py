import ctypes
from ctypes import *
from pydantic import BaseModel

MAX_REPLY_MESSAGE_LENGTH = 256

class StatusMCPPayload(BaseModel):
    success: bool = False
    message: str = ""

class Status(ctypes.Structure):
    type MCPPayload = StatusMCPPayload
    _fields_ = [
        ("success", ctypes.c_bool),
        ("message", ctypes.c_char * MAX_REPLY_MESSAGE_LENGTH),
    ]

    def toString(self):
        return f"(Status: success = {self.success}, message = {self.message.decode('utf-8')})"

    def toMCPPayload(self) -> MCPPayload:
        return StatusMCPPayload(success = self.success, message = self.message.decode('utf-8'))

class TransformMCPPayload(BaseModel):
    positionX: float = 0
    positionY: float = 0
    scaleX: float = 0
    scaleY: float = 0
    angle: float = 0

class Transform(ctypes.Structure):
    type MCPPayload = TransformMCPPayload
    _fields_ = [
        ("positionX", ctypes.c_float),
        ("positionY", ctypes.c_float),
        ("scaleX", ctypes.c_float),
        ("scaleY", ctypes.c_float),
        ("angle", ctypes.c_float)
    ]

    def toString(self):
        return f"(Transform: positionX = {self.positionX}, positionY = {self.positionY}, scaleX = {self.scaleX}, scaleY = {self.scaleY}, angle = {self.angle})"

    def toMCPPayload(self):
        return TransformMCPPayload(
            positionX = self.positionX, 
            positionY = self.positionY,
            scaleX = self.scaleX, 
            scaleY = self.scaleY, 
            angle = self.angle)