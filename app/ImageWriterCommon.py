import ctypes
from ctypes import *
from pydantic import BaseModel, Field

MAX_REPLY_MESSAGE_LENGTH = 256

######## Status ########
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

######## Transform ########
TRANSFORM_DOC = (
    "A Transform consists of:" 
    "positionX (float) = X position, " 
    "positionY (float)= Y position "
    "scaleX (float) = X scale, "
    "scaleY (float) = Y scale"
    "angle (float) = counter clockwise angle orientation on world plane"
)
class TransformMCPPayload(BaseModel):
    positionX: float = Field(..., description="(float), X position")
    positionY: float = Field(..., description="(float), Y position")
    scaleX: float = Field(..., description="(float), X scale")
    scaleY: float = Field(..., description="(float), Y scale")
    angle: float = Field(..., description="(float), counter clockwise angle orientation")

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

    def fromMCPPayload(self, payload):
        self.positionX = payload.positionX
        self.positionY = payload.positionY
        self.scaleX = payload.scaleX 
        self.scaleY = payload.scaleY 
        self.angle = payload.angle

######## Color ########
COLOR_DOC = (
    "A Color consists of:" 
    "red (float) = red channel value " 
    "green (float)= green channel value "
    "blue (float) = blue channel value "
    "alpha (float) = alpha channel value"
    "For each channel, 0 = no strength and 1 = max strength"
)
class ColorMCPPayload(BaseModel):
    red: float = Field(..., description="(float), red channel value")
    green: float = Field(..., description="(float), green channel value")
    blue: float = Field(..., description="(float), blue channel value")
    alpha: float = Field(..., description="(float), alpha channel value")

class Color(ctypes.Structure):
    _fields_ = [
        ("red", ctypes.c_float),
        ("green", ctypes.c_float),
        ("blue", ctypes.c_float),
        ("alpha", ctypes.c_float),
    ]

    def toString(self):
        return f"(Color: red = {self.red}, green = {self.green}, blue = {self.blue}, alpha = {self.alpha})"

    def toMCPPayload(self):
        return TransformMCPPayload(
            red = self.red, 
            green = self.green,
            blue = self.blue, 
            alpha = self.alpha)

    def fromMCPPayload(self, payload):
        self.red = payload.red
        self.green = payload.green
        self.blue = payload.blue 
        self.alpha = payload.alpha 
