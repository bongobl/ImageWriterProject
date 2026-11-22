import ctypes
from ctypes import *
from pydantic import BaseModel, Field
from enum import IntEnum
from typing import Literal, Union, Annotated, Any


MAX_REPLY_MESSAGE_LENGTH = 512
ROOT_ENTITY_ID = 0x0

######## Vec2 ########
class Vec2MCPPayload(BaseModel):
    x: float = Field(..., description="x spatial component")
    y: float = Field(..., description="y spatial component")

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
    position: Vec2MCPPayload = Field(..., description="position")
    orientation: float = Field(..., description="counter clockwise angle orientation in degrees")
    scale: float = Field(..., description="scale")

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

TransformApplyModeModel = Literal["absolute", "offset"]
TransformSpaceModelForEntityAdding = Literal["local", "world"]
TransformSpaceModelForModifying = Literal["self", "self-positionOffsetInWorldUnits", "local", "world"]

MODIFY_TRANSFORM_DESC = (
    "The space in which to apply the new transform properties if being modified, required only if either position/rotation/scale are provided"
    "Choose 'self' for this transform to be applied completely in the entity's own transform space, "
    "useful for moving the entity along one of its own axes. "
    "Choose 'self-positionOffsetInWorldUnits' for this transform to be applied in the entity's own transform space "
    "with the positional offset being applied in world units, "
    "useful for moving the entity along one of its own axes in world units. "
    "Choose 'local' for this transform to be applied in the space of the entity's parent. "
    "Choose 'world' to apply it in world space"
)
class TransformApplyModes(IntEnum):
    NONE = 0x0
    POSITION_AS_OFFSET = 1 << 0x0
    ORIENTATION_AS_OFFSET = 1 << 0x1
    SCALE_AS_OFFSET = 1 << 0x2

class ModifyPositionModel(BaseModel):
    value: Vec2MCPPayload = Field(..., description="positional value used for modification")
    applyMode: TransformApplyModeModel = Field(..., description="choose 'absolute' to override the current position, choose 'offset' to add to it")
    
class ModifyOrientationModel(BaseModel):
    value: float = Field(..., description="orientation value used for modification")
    applyMode: TransformApplyModeModel = Field(..., description="choose 'absolute' to override the current orientation, choose 'offset' to add to it")

class ModifyScaleModel(BaseModel):
    value: float = Field(..., description="scale value used for modification")
    applyMode: TransformApplyModeModel = Field(..., description="choose 'absolute' to override the current scale, choose 'offset' to apply this one to the current scale")


class TransformSpace(IntEnum):
    SELF = 0
    SELF_WITH_POSITION_OFFSET_IN_WORLD_UNITS = 1
    LOCAL = 2
    WORLD = 3

class TransformModelForEntityAdding(BaseModel):
    value: TransformMCPPayload = Field(..., description = "the transform value itself")
    space: TransformSpaceModelForEntityAdding = Field(..., description=
        "choose 'local' for this transform to be applied in the space of the entity's parent"
        "choose 'world' to apply it in world space, if no parent is specified, the transform"
        "will get applied in world space regardless of the space requested"
    )

######## Color ########
class ColorMCPPayload(BaseModel):
    red: float = Field(..., ge=0.0, le=1.0, description="red channel value")
    green: float = Field(..., ge=0.0, le=1.0, description="green channel value")
    blue: float = Field(..., ge=0.0, le=1.0, description="blue channel value")
    alpha: float = Field(..., ge=0.0, le=1.0, description="alpha channel value")

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

######## ScreenSpaceControls ########
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

######## Entity shapes (types) ########

EntityTypeMCPPayload = Literal["null", "ellipse", "rectangle", "triangle"]

class EntityType(IntEnum):
    UNDEFINED = 0
    NULL = 1
    ELLIPSE = 2
    RECTANGLE = 3
    TRIANGLE = 4

# Null
class NullEntityDataMCPPayload(BaseModel):
    kind: Literal["null"] = "null"

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
- kind='null'   -> { kind }   // call for null objects
- kind='ellipse'   -> { kind, halfExtents: Vec2 }   // radii along local x and y
- kind='rectangle' -> { kind, halfExtents: Vec2 }   // half-width and half-height
- kind='triangle'  -> { kind, point1: Vec2, point2: Vec2, point3: Vec2 }  // three vertices in local space"""

ShapeDataUnionMCPPayload = Annotated[
    Union[NullEntityDataMCPPayload, EllipseDataMCPPayload, RectangleDataMCPPayload, TriangleDataMCPPayload],
    Field(discriminator="kind"),
]

TransformPreservationMode = Literal["local", "world"]

class ReparentModel(BaseModel):
    entityId: int = Field(..., description="entity id of the new parent, set it to 0 to put it directly under the world")
    preserveTransform: TransformPreservationMode = Field(..., description="choose 'local' to preserve the child entity's local transform, choose 'world' to preserve its world transform")

class EntityDataModifiable(IntEnum):
    NONE = 0
    TYPE = 1 << 0x0
    POSITION = 1 << 0x1
    ORIENTATION = 1 << 0x2
    SCALE = 1 << 0x3
    COLOR = 1 << 0x4
    Z_ORDER = 1 << 0x5
    HALF_EXTENTS = 1 << 0x6
    TRIANGLE_POINT_1 = 1 << 0x7
    TRIANGLE_POINT_2 = 1 << 0x8
    TRIANGLE_POINT_3 = 1 << 0x9
    PARENT = 1 << 0x10
    REPARENT_PRESERVE_LOCAL_TRANSFORM = 1 << 0x11

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
        ("entityId", ctypes.c_uint64),
        ("parentEntityId", ctypes.c_uint64),
        ("fieldsToModify", ctypes.c_uint32), # stores EntityDataModifiable
        ("transformFieldApplyModes", ctypes.c_uint8), # stores TransformApplyModes
        ("transformSpace", ctypes.c_uint8), # stores TransformSpace
    ]
    _anonymous_ = ("shapeDataUnion",)

    def __init__(
        self, 
        entityType = EntityType.UNDEFINED,
        transform = Transform(),
        color = Color(),
        zOrder = -1,
        ellipse = None,
        rectangle = None,
        triangle = None,
        entityId = ROOT_ENTITY_ID,
        parentEntityId = ROOT_ENTITY_ID,
        fieldsToModify = EntityDataModifiable.NONE,
        transformFieldApplyModes = TransformApplyModes.NONE,
        transformSpace = TransformSpace.LOCAL
    ):
        
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


        self.entityId = entityId
        self.parentEntityId = parentEntityId
        self.fieldsToModify = fieldsToModify
        self.transformFieldApplyModes = transformFieldApplyModes
        self.transformSpace = transformSpace


    def __str__(self):

        typeStr = ""
        match self.entityType:
            case EntityType.UNDEFINED:
                typeStr = "UNDEFINED"
            case EntityType.NULL:
                typeStr = "Null Entity"
            case EntityType.ELLIPSE:
                typeStr = f"{self.ellipse}";
            case EntityType.RECTANGLE:
                typeStr = f"{self.rectangle}";
            case EntityType.TRIANGLE:
                typeStr = f"{self.triangle}";
            case _:
                typeStr = "Unrecognized type value"

        return (
            f"(EntityData: transform = {self.transform}, "
            f"color = {self.color}, "
            f"zOrder = {self.zOrder}, "
            "entity type = " + typeStr + "), "
            f"entityId = {self.entityId}, "
            f"parentEntityId = {self.parentEntityId}, "
            f"fieldsToModify = 0x{self.fieldsToModify:x}, "
            f"transformFieldApplyModes = 0x{self.transformFieldApplyModes:x}, "
            f"transformSpace = 0x{self.transformSpace:x})"
        )

    # creation
    def fromAddEntityMCPPayload(self, transformMod, color, zOrder, shapeDataUnion, parentEntityId):
        self.transform.fromMCPPayload(transformMod.value)
        self.color.fromMCPPayload(color)
        self.zOrder = ctypes.c_int(zOrder)
        
        match shapeDataUnion:
            case NullEntityDataMCPPayload():
                self.entityType = EntityType.NULL
            case EllipseDataMCPPayload():
                self.entityType = EntityType.ELLIPSE
                self.ellipse.fromMCPPayload(shapeDataUnion)
            case RectangleDataMCPPayload():
                self.entityType = EntityType.RECTANGLE
                self.rectangle.fromMCPPayload(shapeDataUnion)
            case TriangleDataMCPPayload():
                self.entityType = EntityType.TRIANGLE
                self.triangle.fromMCPPayload(shapeDataUnion)

        self.parentEntityId = ctypes.c_uint64(parentEntityId) if parentEntityId is not None else ROOT_ENTITY_ID
        self.transformSpace = TransformSpace.LOCAL if (transformMod.space == "local") else TransformSpace.WORLD

    # modification
    def fromModifyEntityMCPPayload(self, entityId, shapeType, positionMod, orientationMod, scaleMod, transformSpaceMod, color, zOrder, halfExtents, trianglePoint1, trianglePoint2, trianglePoint3, newParentMod):
     
        self.entityId = ctypes.c_uint64(entityId)

        errorsMessages = []

        # validate shape arguments
        if shapeType is not None:
            self.fieldsToModify |= EntityDataModifiable.TYPE
            match shapeType:
                case "null":
                    if trianglePoint1 is not None or trianglePoint2 is not None or trianglePoint3 is not None or halfExtents is not None:
                        errorsMessages.append("None of the 'trianglePoint<N>' or 'halfExtents' properties should be provided when setting entity shape to null")
                    self.entityType = EntityType.NULL
                case "ellipse":
                    if halfExtents is None:
                        errorsMessages.append("'halfExtents' field must be provided to set entity shape to ellipse")
                    if trianglePoint1 is not None or trianglePoint2 is not None or trianglePoint3 is not None:
                        errorsMessages.append("None of the 'trianglePoint<N>' properties should be provided when setting entity shape to ellipse")

                    self.entityType = EntityType.ELLIPSE
                case "rectangle":
                    if halfExtents is None:
                        errorsMessages.append("'halfExtents' field must be provided to set entity shape to rectangle")
                    if trianglePoint1 is not None or trianglePoint2 is not None or trianglePoint3 is not None:
                        errorsMessages.append("None of the 'trianglePoint<N>' properties should be provided when setting entity shape to rectangle")

                    self.entityType = EntityType.RECTANGLE
                case "triangle":
                    if trianglePoint1 is None or trianglePoint2 is None or trianglePoint3 is None:
                        errorsMessages.append("All three 'trianglePoint<N>' properties must be provided to set entity shape to triangle")
                    if halfExtents is not None:
                        errorsMessages.append("'halfExtents' should not be provided when setting entity shape to triangle")

                    self.entityType = EntityType.TRIANGLE

        # validate transform arguments
        if positionMod is not None or orientationMod is not None or scaleMod is not None:
            if transformSpaceMod is None:
                errorsMessages.append("'transformSpace' must be provided if modifying any transform properties (position, rotation, scale)")

        if len(errorsMessages) > 0:
            return PlainReplyMCPPayload(success=False, message="\n".join(errorsMessages))

        if positionMod is not None:
            self.fieldsToModify |= EntityDataModifiable.POSITION
            self.transform.position.fromMCPPayload(positionMod.value)
            self.transformFieldApplyModes |=  TransformApplyModes.POSITION_AS_OFFSET if (positionMod.applyMode == "offset") else TransformApplyModes.NONE

        if orientationMod is not None:
            self.fieldsToModify |= EntityDataModifiable.ORIENTATION
            self.transform.orientation = ctypes.c_float(orientationMod.value)
            self.transformFieldApplyModes |=  TransformApplyModes.ORIENTATION_AS_OFFSET if (orientationMod.applyMode == "offset") else TransformApplyModes.NONE

        if scaleMod is not None:
            self.fieldsToModify |= EntityDataModifiable.SCALE
            self.transform.scale = ctypes.c_float(scaleMod.value)
            self.transformFieldApplyModes |=  TransformApplyModes.SCALE_AS_OFFSET if (scaleMod.applyMode == "offset") else TransformApplyModes.NONE

        if transformSpaceMod is not None:
            match transformSpaceMod:
                case "self":
                    self.transformSpace = TransformSpace.SELF 
                case "self-positionOffsetInWorldUnits":
                    self.transformSpace = TransformSpace.SELF_WITH_POSITION_OFFSET_IN_WORLD_UNITS 
                case "local":
                    self.transformSpace = TransformSpace.LOCAL 
                case "world":
                    self.transformSpace = TransformSpace.WORLD
                
        if color is not None:
            self.fieldsToModify |= EntityDataModifiable.COLOR
            self.color.fromMCPPayload(color)

        if zOrder is not None:
            self.fieldsToModify |= EntityDataModifiable.Z_ORDER
            self.zOrder = ctypes.c_int(zOrder)

        if halfExtents is not None:
            self.fieldsToModify |= EntityDataModifiable.HALF_EXTENTS
            self.ellipse.halfExtents.fromMCPPayload(halfExtents)

        if trianglePoint1 is not None:
            self.fieldsToModify |= EntityDataModifiable.TRIANGLE_POINT_1
            self.triangle.point1.fromMCPPayload(trianglePoint1)

        if trianglePoint2 is not None:
            self.fieldsToModify |= EntityDataModifiable.TRIANGLE_POINT_2
            self.triangle.point2.fromMCPPayload(trianglePoint2)

        if trianglePoint3 is not None:
            self.fieldsToModify |= EntityDataModifiable.TRIANGLE_POINT_3
            self.triangle.point3.fromMCPPayload(trianglePoint3)

        if newParentMod is not None:
            self.fieldsToModify |= EntityDataModifiable.PARENT
            self.fieldsToModify |=  EntityDataModifiable.REPARENT_PRESERVE_LOCAL_TRANSFORM if (newParentMod.preserveTransform == "local") else EntityDataModifiable.NONE
            self.parentEntityId = ctypes.c_uint64(newParentMod.entityId)

        return PlainReplyMCPPayload(success=True, message="")

              

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

    def toMCPPayload(self):

        if self.success:
            return AddEntityReplyMCPPayload(
                success = self.success, 
                message = self.message.decode('utf-8'),
                entityId = self.entityId,
                zOrder = self.zOrder,
            )
        else:
            return PlainReplyMCPPayload(
                success = self.success, 
                message = self.message.decode('utf-8'),
            )

class EntityDataReplyMCPPayload(PlainReplyMCPPayload):
    transform: TransformMCPPayload
    color: ColorMCPPayload
    zOrder: int
    parentEntityId: int

class NullEntityDataReplyMCPPayload(EntityDataReplyMCPPayload):
    shape: EntityTypeMCPPayload = "null"

class EllipseDataReplyMCPPayload(EntityDataReplyMCPPayload):
    shape: EntityTypeMCPPayload = "ellipse"
    halfExtents: Vec2MCPPayload

class RectangleDataReplyMCPPayload(EntityDataReplyMCPPayload):
    shape: EntityTypeMCPPayload = "rectangle"
    halfExtents: Vec2MCPPayload
class TriangleDataReplyMCPPayload(EntityDataReplyMCPPayload):
    shape: EntityTypeMCPPayload = "triangle"
    point1: Vec2MCPPayload
    point2: Vec2MCPPayload
    point3: Vec2MCPPayload

class EntityDataReply(ctypes.Structure):
    
    _fields_ = [
        ("success", ctypes.c_bool),
        ("message", ctypes.c_char * MAX_REPLY_MESSAGE_LENGTH),
        ("entityData", EntityData),
    ]

    def __init__(self, success = False, message = b"UNINITIALIZED_MESSAGE", entityData = EntityData()):
        self.success = success
        self.message = message
        self.entityData = entityData

    def __str__(self):
        return f"(EntityDataReply: success = {self.success}, message = {self.message.decode('utf-8')}, entityData = {self.entityData})"

    def toMCPPayload(self):
            
        match self.entityData.entityType:
            case EntityType.UNDEFINED:
                return PlainReplyMCPPayload(
                    success = self.success, 
                    message = self.message.decode('utf-8'),
                )
            case EntityType.NULL:
                return NullEntityDataReplyMCPPayload(
                    success = self.success,
                    message = self.message.decode('utf-8'),
                    transform = self.entityData.transform.toMCPPayload(),
                    color = self.entityData.color.toMCPPayload(),
                    zOrder = int(self.entityData.zOrder),
                    parentEntityId = self.entityData.parentEntityId
                )
            case EntityType.ELLIPSE:
                return EllipseDataReplyMCPPayload(
                    success = self.success,
                    message = self.message.decode('utf-8'),
                    transform = self.entityData.transform.toMCPPayload(),
                    color = self.entityData.color.toMCPPayload(),
                    zOrder = int(self.entityData.zOrder),
                    parentEntityId = self.entityData.parentEntityId,
                    halfExtents = self.entityData.ellipse.halfExtents.toMCPPayload()
                )
            case EntityType.RECTANGLE:
                return RectangleDataReplyMCPPayload(
                    success = self.success, 
                    message = self.message.decode('utf-8'),
                    transform = self.entityData.transform.toMCPPayload(),
                    color = self.entityData.color.toMCPPayload(),
                    zOrder = int(self.entityData.zOrder),
                    parentEntityId = self.entityData.parentEntityId,
                    halfExtents = self.entityData.rectangle.halfExtents.toMCPPayload()
                )
            case EntityType.TRIANGLE:
                return TriangleDataReplyMCPPayload(
                    success = self.success, 
                    message = self.message.decode('utf-8'),
                    transform = self.entityData.transform.toMCPPayload(),
                    color = self.entityData.color.toMCPPayload(),
                    zOrder = int(self.entityData.zOrder),
                    parentEntityId = self.entityData.parentEntityId,
                    point1 = self.entityData.triangle.point1.toMCPPayload(),
                    point2 = self.entityData.triangle.point2.toMCPPayload(),
                    point3 = self.entityData.triangle.point3.toMCPPayload(),
                )
            case _:
                return PlainReplyMCPPayload(
                    success = False, 
                    message = "Unknown shape type received by the app",
                )

class NumEntitiesReply(ctypes.Structure):

    _fields_ = [
        ("success", ctypes.c_bool),
        ("message", ctypes.c_char * MAX_REPLY_MESSAGE_LENGTH),
        ("numEntities", ctypes.c_uint64),
    ]

    def __init__(self, success = False, message = b"UNINITIALIZED_MESSAGE", numEntities = 0):
        self.success = success
        self.message = message
        self.numEntities = numEntities

    def __str__(self):
        return f"(NumEntitiesReply: success = {self.success}, message = {self.message.decode('utf-8')}, numEntities = {self.numEntities})"

class SceneDataReplyModel(PlainReplyMCPPayload):
    cameraTransform: TransformMCPPayload
    cameraWidthFromHeight: float = Field(..., description="(float), aspect ratio of the camera view, ie the width / height")
    numEntities: int
    entities: list[Any]
    