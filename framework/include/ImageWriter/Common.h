#pragma once
#define MAX_REPLY_MESSAGE_LENGTH 512
#include <stdint.h>
struct Vec2 {
    float x = 0;
    float y = 0;
};

// TODO: don't let scale go below or equal to 0, 
// rename angle to orientation and flop order with scale
struct Transform {
    Vec2 position = { 0,0 };
    float orientation = 0;
    float scale = 1;
};

enum TransformApplyModes {
    TRANSFORM_APPLY_MODE_POSITION_AS_OFFSET = 1 << 0x0,
    TRANSFORM_APPLY_MODE_ORIENTATION_AS_OFFSET = 1 << 0x1,
    TRANSFORM_APPLY_MODE_SCALE_AS_OFFSET = 1 << 0x2,
};

struct Color {
    float red = 0;
    float green = 0;
    float blue = 0;
    float alpha = 0;
};

struct ScreenSpaceControls {
    int moveX = 0;
    int moveY = 0;
    int rotate = 0;
    int zoom = 0;
};

enum EntityType {
    ENTITY_TYPE_NONE = 0,
    ENTITY_TYPE_ELLIPSE = 1,
    ENTITY_TYPE_RECTANGLE = 2,
    ENTITY_TYPE_TRIANGLE = 3,
};

struct EllipseData {
    Vec2 halfExtents;
};

typedef EllipseData RectangleData;

struct TriangleData {
    Vec2 point1;
    Vec2 point2;
    Vec2 point3;
};

enum EntityDataModifiable {
    ENTITY_DATA_MODIFIABLE_TYPE = 1 << 0x0,
    ENTITY_DATA_MODIFIABLE_POSITION = 1 << 0x1,
    ENTITY_DATA_MODIFIABLE_ORIENTATION = 1 << 0x2,
    ENTITY_DATA_MODIFIABLE_SCALE = 1 << 0x3,
    ENTITY_DATA_MODIFIABLE_COLOR = 1 << 0x4,
    ENTITY_DATA_MODIFIABLE_Z_ORDER = 1 << 0x5,
    ENTITY_DATA_MODIFIABLE_HALF_EXTENTS = 1 << 0x6,
    ENTITY_DATA_MODIFIABLE_TRIANGLE_POINT_1 = 1 << 0x7,
    ENTITY_DATA_MODIFIABLE_TRIANGLE_POINT_2 = 1 << 0x8,
    ENTITY_DATA_MODIFIABLE_TRIANGLE_POINT_3 = 1 << 0x9,
};

struct EntityData {
    int entityType = ENTITY_TYPE_NONE;
    Transform transform;
    Color color;
    int zOrder = -1;
    
    union {
        EllipseData ellipse;
        RectangleData rectangle;
        TriangleData triangle;
    };

    uint64_t entityIdToModify = 0;
    uint32_t fieldsToModify = 0x0;
    uint8_t transformFieldApplyModes = 0x0;
};

// Replies
struct PlainReply {
    bool success = false;
    char message[MAX_REPLY_MESSAGE_LENGTH] = "UNINITIALIZED_MESSAGE";
};

struct TransformReply {
    bool success = false;
    char message[MAX_REPLY_MESSAGE_LENGTH] = "UNINITIALIZED_MESSAGE";
    Transform transform;
    float cameraWidthFromHeight = 0;
};

struct AddEntityReply {
    bool success = false;
    char message[MAX_REPLY_MESSAGE_LENGTH] = "UNINITIALIZED_MESSAGE";
    uint64_t entityId = -1;
    int zOrder = -1;
};
struct EntityDataReply {
    bool success = false;
    char message[MAX_REPLY_MESSAGE_LENGTH] = "UNINITIALIZED_MESSAGE";
    EntityData entityData;
};