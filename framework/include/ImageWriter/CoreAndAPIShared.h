#pragma once
#include <windows.h>
#include <cstdint>
#include <ImageWriter/API.h>

// Included by API.cpp, No constructs allowed here


struct InstanceData;
typedef bool (*PfnCoreInitialize)(InstanceData*);

typedef bool (*PfnCoreGetCameraView)(InstanceData, RectParams*);
typedef bool (*PfnCoreDrawCircle)(InstanceData, float, float, float);
typedef bool (*PfnCoreDrawRectangle)(InstanceData, float, float, float, float);
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

    PfnCoreGetCameraView pfnCoreGetCameraView;
    PfnCoreDrawCircle pfnCoreDrawCircle;
    PfnCoreDrawRectangle pfnCoreDrawRectangle;
    PfnCoreClearImage pfnCoreClearImage;

    PfnCoreInitRenderWindow pfnCoreInitRenderWindow;
    PfnCoreUpdateRenderWindow pfnCoreUpdateRenderWindow;
    PfnCoreDisposeRenderWindow pfnCoreDisposeRenderWindow;
    PfnIsIsolatedRenderWindowOpen pfnIsIsolatedRenderWindowOpen;
    PfnCoreDispose pfnCoreDispose;

    void* pCoreData;
    char* pPublicStatusMessage;
};