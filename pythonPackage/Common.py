import ctypes
from ctypes import *
from pydantic import BaseModel, Field
from enum import IntEnum
from typing import Literal, Union, Annotated
MAX_REPLY_MESSAGE_LENGTH = 512

######## Vec2 ########
class Vec2MCPPayload(BaseModel):
    x: float = Field(..., description="(float), x spatial component")
    y: float = Field(..., description="(float), y spatial component")

class Vec2(ctypes.Structure):
    _fields_ = [
        ("x", ctypes.c_float),
        ("y", ctypes.c_float),
    ]

    def __init__(self, x = 0, y = 0):
        self.x = x
        self.y = y

    def __str__(self):
        return f"(Vec2: x = {self.x}, y = {self.y})"

    def toMCPPayload(self) -> Vec2MCPPayload:
        return Vec2MCPPayload(
            x = self.x, 
            y = self.y
        )

    def fromMCPPayload(self, payload):
        self.x = payload.x
        self.y = payload.y

######## Transform ########
class TransformMCPPayload(BaseModel):
    position: Vec2MCPPayload = Field(..., description="(Vec2MCPPayload), position")
    orientation: float = Field(..., description="(float), counter clockwise angle orientation in degrees")
    scale: float = Field(..., description="(float), scale")

class Transform(ctypes.Structure):

    _fields_ = [
        ("position", Vec2),
        ("orientation", ctypes.c_float),
        ("scale", ctypes.c_float)
    ]

    def __init__(self, position = Vec2(0,0), orientation = 0, scale = 0):
        self.position = position
        self.orientation = orientation
        self.scale = scale

    def __str__(self):
        return f"(Transform: position = {self.position}, orientation = {self.orientation}, scale = {self.scale})"

    def toMCPPayload(self) -> TransformMCPPayload:
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
class ColorMCPPayload(BaseModel):
    red: float = Field(..., ge=0.0, le=1.0, description="(float), red channel value")
    green: float = Field(..., ge=0.0, le=1.0, description="(float), green channel value")
    blue: float = Field(..., ge=0.0, le=1.0, description="(float), blue channel value")
    alpha: float = Field(..., ge=0.0, le=1.0, description="(float), alpha channel value")

class Color(ctypes.Structure):

    _fields_ = [
        ("red", ctypes.c_float),
        ("green", ctypes.c_float),
        ("blue", ctypes.c_float),
        ("alpha", ctypes.c_float),
    ]

    def __init__(self, red = 0, green = 0, blue = 0, alpha = 0):
        self.red = red
        self.green = green
        self.blue = blue
        self.alpha = alpha
        
    def __str__(self):
        return f"(Color: red = {self.red}, green = {self.green}, blue = {self.blue}, alpha = {self.alpha})"

    def toMCPPayload(self) -> ColorMCPPayload:
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

class ScreenSpaceControls(ctypes.Structure):

    _fields_ = [
        ("moveX", ctypes.c_int),
        ("moveY", ctypes.c_int),
        ("rotate", ctypes.c_int),
        ("zoom", ctypes.c_int),
    ]

    def __init__(self, moveX = 0, moveY = 0, rotate = 0, zoom = 0):
        self.moveX = moveX
        self.moveY = moveY
        self.rotate = rotate
        self.zoom = zoom

class EntityType(IntEnum):
    NONE = 0
    ELLIPSE = 1
    RECTANGLE = 2
    TRIANGLE = 3

# Ellipse
class EllipseDataMCPPayload(BaseModel):
    kind: Literal["ellipse"] = "ellipse"
    halfExtents: Vec2MCPPayload

class EllipseData(ctypes.Structure):
    _fields_ = [
        ("halfExtents", Vec2),
    ]

    def __str__(self):
        return f"(EllipseData: halfExtents = {self.halfExtents})"

    def fromMCPPayload(self, payload):
        self.halfExtents.fromMCPPayload(payload.halfExtents)

# Rectangle
class RectangleDataMCPPayload(BaseModel):
    kind: Literal["rectangle"] = "rectangle"
    halfExtents: Vec2MCPPayload

class RectangleData(ctypes.Structure):
    _fields_ = [
        ("halfExtents", Vec2),
    ]

    def __str__(self):
        return f"(RectangleData: halfExtents = {self.halfExtents})"

    def fromMCPPayload(self, payload):
        self.halfExtents.fromMCPPayload(payload.halfExtents)

# Triangle
class TriangleDataMCPPayload(BaseModel):
    kind: Literal["triangle"] = "triangle"
    point1: Vec2MCPPayload
    point2: Vec2MCPPayload
    point3: Vec2MCPPayload

class TriangleData(ctypes.Structure):
    _fields_ = [
        ("point1", Vec2),
        ("point2", Vec2),
        ("point3", Vec2),
    ]

    def __str__(self):
        return f"(TriangleData: point1 = {self.point1}, point2 = {self.point2}, point3 = {self.point3})"

    def fromMCPPayload(self, payload):
        self.point1.fromMCPPayload(payload.point1)
        self.point2.fromMCPPayload(payload.point2)
        self.point3.fromMCPPayload(payload.point3)

# Entity common

Z_ORDER_DOC = (
    "The Z order of an entity represents how 'high up' off the 2D world it is, which dictates which entites it is drawn in front of"
    "and which ones it is drawn behind. A higher Z order represents a higher value off the 2D world. The Z orders for all N entites"
    "in the current scene starts at 0 and is contiguous up to all N-1, meaning you can insert a new entity 'A' between an existing"
    "entity 'B' and the entity just behind it by specifying the Z order of 'B' when creating 'A'. This follows an array list behavior"
    "where insertion and removal from the middle of the list causes elements of indices above it to shift accordingly. Note here that"
    "specifying a Z order of -1 places the new entity at the top, giving it the highest Z order for all existing entities"
)

SHAPE_DATA_UNION_DOC = """Geometry for the entity. A discriminated union selected by the required string field 'kind'. (Vec2 = { x: float, y: float }.)
- kind='ellipse'   -> { kind, halfExtents: Vec2 }   // radii along local x and y
- kind='rectangle' -> { kind, halfExtents: Vec2 }   // half-width and half-height
- kind='triangle'  -> { kind, point1: Vec2, point2: Vec2, point3: Vec2 }  // three vertices in local space"""

ShapeDataUnionMCPPayload = Annotated[
    Union[EllipseDataMCPPayload, RectangleDataMCPPayload, TriangleDataMCPPayload],
    Field(discriminator="kind"),
]

class ShapeDataUnion(ctypes.Union):
    _fields_ = [
        ("ellipse", EllipseData),
        ("rectangle", RectangleData),
        ("triangle", TriangleData),
    ]

class EntityData(ctypes.Structure):
    _fields_ = [
        ("entityType", ctypes.c_int),
        ("transform", Transform),
        ("color", Color),
        ("zOrder", ctypes.c_int),
        ("shapeDataUnion", ShapeDataUnion),
    ]
    _anonymous_ = ("shapeDataUnion",)

    def __init__(self, entityType = EntityType.NONE, transform = Transform(), color = Color(), zOrder = -1, ellipse = None, rectangle = None, triangle = None):
        
        self.entityType = entityType
        self.transform = transform
        self.color = color
        self.zOrder = zOrder

        match(entityType):
            case EntityType.ELLIPSE:   
                self.ellipse = ellipse
            case EntityType.RECTANGLE:   
                self.rectangle = rectangle
            case EntityType.TRIANGLE:   
                self.triangle = triangle


    def __str__(self):

        typeStr = ""
        match self.entityType:
            case EntityType.NONE:
                typeStr = "NONE"
            case EntityType.ELLIPSE:
                typeStr = f"{self.ellipse}";
            case EntityType.RECTANGLE:
                typeStr = f"{self.rectangle}";
            case EntityType.TRIANGLE:
                typeStr = f"{self.triangle}";
            case _:
                typeStr = "Unrecognized type value"

        return f"(EntityData: transform = {self.transform}, color = {self.color}, zOrder = {self.zOrder}, entity type = " + typeStr + ")"

    def fromMCPPayload(self, transform, color, zOrder, shapeDataUnion):
        self.transform.fromMCPPayload(transform)
        self.color.fromMCPPayload(color)
        self.zOrder = ctypes.c_int(zOrder)
        
        match shapeDataUnion:
            case EllipseDataMCPPayload():
                self.entityType = EntityType.ELLIPSE
                self.ellipse.fromMCPPayload(shapeDataUnion)
            case RectangleDataMCPPayload():
                self.entityType = EntityType.RECTANGLE
                self.rectangle.fromMCPPayload(shapeDataUnion)
            case TriangleDataMCPPayload():
                self.entityType = EntityType.TRIANGLE
                self.triangle.fromMCPPayload(shapeDataUnion)
        

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

    def __init__(self, success = False, message = b"UNINITIALIZED_MESSAGE"):
        self.success = success
        self.message = message

    def __str__(self):
        return f"(PlainReply: success = {self.success}, message = {self.message.decode('utf-8')})"

    def toMCPPayload(self) -> PlainReplyMCPPayload:
        return PlainReplyMCPPayload(success = self.success, message = self.message.decode('utf-8'))

# Transform (additional MCPPayload type for Camera transform)
class TransformReplyMCPPayload(PlainReplyMCPPayload):
    transform: TransformMCPPayload

class CameraTransformReplyMCPPayload(TransformReplyMCPPayload):
    widthFromHeight: float = Field(..., description="(float), aspect ratio of the camera view, ie the width / height")

class TransformReply(ctypes.Structure):

    _fields_ = [
        ("success", ctypes.c_bool),
        ("message", ctypes.c_char * MAX_REPLY_MESSAGE_LENGTH),
        ("transform", Transform),
        ("cameraWidthFromHeight", ctypes.c_float), # only used when getting camera transform
    ]

    def __init__(self, success = False, message = b"UNINITIALIZED_MESSAGE", transform = Transform(), cameraWidthFromHeight = 0):
        self.success = success
        self.message = message
        self.transform = transform
        self.cameraWidthFromHeight = cameraWidthFromHeight

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
# Entity
class AddEntityReplyMCPPayload(PlainReplyMCPPayload):
    entityId: int
    zOrder: int

class AddEntityReply(ctypes.Structure):
    _fields_ = [
        ("success", ctypes.c_bool),
        ("message", ctypes.c_char * MAX_REPLY_MESSAGE_LENGTH),
        ("entityId", ctypes.c_uint64),
        ("zOrder", ctypes.c_int),
    ]

    def __init__(self, success = False, message = b"UNINITIALIZED_MESSAGE", entityId = -1, zOrder = -1):
        self.success = success
        self.message = message
        self.entityId = entityId
        self.zOrder = zOrder

    def __str__(self):
        return f"(AddEntityReply: success = {self.success}, message = {self.message.decode('utf-8')}, entityId = {self.entityId}, zOrder = {self.zOrder})"

    def toMCPPayload(self) -> AddEntityReplyMCPPayload:

        return AddEntityReplyMCPPayload(
            success = self.success, 
            message = self.message.decode('utf-8'),
            entityId = self.entityId,
            zOrder = self.zOrder,
        )