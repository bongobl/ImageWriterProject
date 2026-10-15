#pragma once
#include <stdio.h>
#include <cstdint>



struct Framework
{
    void* pData;
};

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

enum GridViewMode {
    GRID_VIEW_MODE_NONE = 0,
    GRID_VIEW_MODE_FOREGROUND = 1,
    GRID_VIEW_MODE_BACKGROUND = 2,
};

struct RenderConfig {
    int gridViewMode;
};

typedef Framework HImageWriterInstance;

// instance
extern "C" __declspec(dllexport) bool ImageWriter_Instance_Intialize(HImageWriterInstance* pInstance);
extern "C" __declspec(dllexport) bool ImageWriter_Instance_Dispose(HImageWriterInstance* pInstance);

// scene lifecycle
extern "C" __declspec(dllexport) bool ImageWriter_Scene_Init(HImageWriterInstance instance, int64_t windowHandle, RenderConfig renderConfig);
extern "C" __declspec(dllexport) bool ImageWriter_Scene_Dispose(HImageWriterInstance instance);
extern "C" __declspec(dllexport) bool ImageWriter_Scene_UpdateFrame(HImageWriterInstance instance, float deltaTime);
extern "C" __declspec(dllexport) bool ImageWriter_Scene_IsSelfManagedRenderWindowOpen(HImageWriterInstance instance);
extern "C" __declspec(dllexport) bool ImageWriter_Scene_UpdateRenderConfig(HImageWriterInstance instance, RenderConfig renderConfig);

// scene API
extern "C" __declspec(dllexport) bool ImageWriter_Scene_SetCameraTransform(HImageWriterInstance instance, char* pStatusMessage, Transform transform);
extern "C" __declspec(dllexport) bool ImageWriter_Scene_GetCameraTransform(HImageWriterInstance instance, char* pStatusMessage, Transform* pCameraTransform, float* pWidthFromheight);
extern "C" __declspec(dllexport) bool ImageWriter_Scene_TEMP_MoveCameraLocalSpace(HImageWriterInstance instance, char* pStatusMessage, Transform delta);
extern "C" __declspec(dllexport) bool ImageWriter_Scene_AddEntity(HImageWriterInstance instance, char* pStatusMessage, AddEntityParams params);
extern "C" __declspec(dllexport) bool ImageWriter_Scene_RemoveAllEntities(HImageWriterInstance instance, char* pStatusMessage);



