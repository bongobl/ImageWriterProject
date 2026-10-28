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