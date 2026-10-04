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

    def __str__(self):
        return f"(Status: success = {self.success}, message = {self.message.decode('utf-8')})"

    def toMCPPayload(self) -> MCPPayload:
        return StatusMCPPayload(success = self.success, message = self.message.decode('utf-8'))

######## Vec2 ########
VEC2_DOC = (
    "A Vec2 consists of: " 
    "x (float) = x spatial component, " 
    "y (float)= y spatial component"
)
class Vec2MCPPayload(BaseModel):
    x: float = Field(..., description="(float), x spatial component")
    y: float = Field(..., description="(float), y spatial component")

class Vec2(ctypes.Structure):
    type MCPPayload = Vec2MCPPayload
    _fields_ = [
        ("x", ctypes.c_float),
        ("y", ctypes.c_float),
    ]

    def __str__(self):
        return f"(Vec2: x = {self.x}, y = {self.y})"

    def toMCPPayload(self):
        return Vec2MCPPayload(
            x = self.x, 
            y = self.y
        )

    def fromMCPPayload(self, payload):
        self.x = payload.x
        self.y = payload.y

######## Transform ########
TRANSFORM_DOC = (
    "A Transform consists of: " 
    "position (Vec2MCPPayload) = 2D (x and y) position" 
    "orientation (float) = counter clockwise angle orientation on world plane in degrees, "
    "scale (float) = scale"
)

class TransformMCPPayload(BaseModel):
    position: Vec2MCPPayload = Field(..., description="(Vec2MCPPayload), position")
    orientation: float = Field(..., description="(float), counter clockwise angle orientation in degrees")
    scale: float = Field(..., description="(float), scale")

class Transform(ctypes.Structure):
    type MCPPayload = TransformMCPPayload
    _fields_ = [
        ("position", Vec2),
        ("orientation", ctypes.c_float),
        ("scale", ctypes.c_float)
        
    ]

    def __str__(self):
        return f"(Transform: position = {self.position}, orientation = {self.orientation}, scale = {self.scale})"

    def toMCPPayload(self):
        return TransformMCPPayload(
            position = self.position.toMCPPayload(),
            orientation = self.orientation,
            scale = self.scale
        )

    def fromMCPPayload(self, payload):
        self.position.fromMCPPayload(payload.position)
        self.orientation = payload.orientation
        self.scale = payload.scale 

######## Color ########
COLOR_DOC = (
    "A Color consists of: " 
    "red (float) = red channel value, " 
    "green (float)= green channel value, "
    "blue (float) = blue channel value, "
    "alpha (float) = alpha channel value, "
    "For each channel, 0 = no strength and 1 = max strength"
)
class ColorMCPPayload(BaseModel):
    red: float = Field(..., description="(float), red channel value")
    green: float = Field(..., description="(float), green channel value")
    blue: float = Field(..., description="(float), blue channel value")
    alpha: float = Field(..., description="(float), alpha channel value")

class Color(ctypes.Structure):
    type MCPPayload = ColorMCPPayload
    _fields_ = [
        ("red", ctypes.c_float),
        ("green", ctypes.c_float),
        ("blue", ctypes.c_float),
        ("alpha", ctypes.c_float),
    ]

    def __str__(self):
        return f"(Color: red = {self.red}, green = {self.green}, blue = {self.blue}, alpha = {self.alpha})"

    def toMCPPayload(self):
        return ColorMCPPayload(
            red = self.red, 
            green = self.green,
            blue = self.blue, 
            alpha = self.alpha
        )

    def fromMCPPayload(self, payload):
        self.red = payload.red
        self.green = payload.green
        self.blue = payload.blue 
        self.alpha = payload.alpha 