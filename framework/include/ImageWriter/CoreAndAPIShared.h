#pragma once
#include <windows.h>
#include <cstdint>
#include <ImageWriter/API.h>

// Included by API.cpp, No constructs allowed here


struct InstanceData;
typedef bool (*PfnCoreInitialize)(InstanceData*);

typedef bool (*PfnCoreGetCameraTransform)(InstanceData, Transform*);
typedef bool (*PfnCoreAddEllipse)(InstanceData, Transform, Color);
typedef bool (*PfnCoreAddRectangle)(InstanceData, Transform, Color);
typedef bool (*PfnCoreAddTriangle)(InstanceData, Vec2, Vec2, Vec2, Transform, Color);
typedef bool (*PfnCoreClearImage)(InstanceData);

typedef bool (*PfnCoreInitRenderWindow)(InstanceData, int64_t);
typedef bool (*PfnCoreUpdateRenderWindow)(InstanceData, float);
typedef bool (*PfnCoreDisposeRenderWindow)(InstanceData);
typedef bool (*PfnIsIsolatedRenderWindowOpen)(InstanceData);
typedef bool (*PfnCoreDispose)(InstanceData*);

struct InstanceData
{
    HMODULE hDll;

    PfnCoreInitialize pfnCoreInitialize;

    PfnCoreGetCameraTransform pfnCoreGetCameraTransform;
    PfnCoreAddEllipse pfnCoreAddEllipse;
    PfnCoreAddRectangle pfnCoreAddRectangle;
    PfnCoreAddTriangle pfnCoreAddTriangle;
    PfnCoreClearImage pfnCoreClearImage;

    PfnCoreInitRenderWindow pfnCoreInitRenderWindow;
    PfnCoreUpdateRenderWindow pfnCoreUpdateRenderWindow;
    PfnCoreDisposeRenderWindow pfnCoreDisposeRenderWindow;
    PfnIsIsolatedRenderWindowOpen pfnIsIsolatedRenderWindowOpen;
    PfnCoreDispose pfnCoreDispose;

    void* pCoreData;
    char* pPublicStatusMessage;
};