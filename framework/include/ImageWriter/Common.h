#pragma once
#define MAX_REPLY_MESSAGE_LENGTH 512

struct Vec2 {
    float x;
    float y;
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

// Parameters
enum EntityType {
    ENTITY_TYPE_NONE = 0,
    ENTITY_TYPE_ELLIPSE = 1,
    ENTITY_TYPE_RECTANGLE = 2,
    ENTITY_TYPE_TRIANGLE = 3,
};

struct AddEllipseParams {
    Vec2 halfExtents;
};

typedef AddEllipseParams AddRectangleParams;

struct AddTriangleParams {
    Vec2 point1;
    Vec2 point2;
    Vec2 point3;
};

struct AddEntityParams {
    int entityType = 0;
    Transform transform;
    Color color;

    union {
        AddEllipseParams ellipse;
        AddRectangleParams rectangle;
        AddTriangleParams triangle;
    };
};

// Replies
struct PlainReply {
    bool success;
    char message[MAX_REPLY_MESSAGE_LENGTH];
};

struct TransformReply {
    bool success;
    char message[MAX_REPLY_MESSAGE_LENGTH];
    Transform transform;
    float cameraWidthFromHeight;
};