#pragma once
#include <windows.h>
#include <cstdint>
// Included by API.cpp, No constructs allowed here


struct InstanceData;
typedef bool (*PfnCoreInitialize)(InstanceData*);
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