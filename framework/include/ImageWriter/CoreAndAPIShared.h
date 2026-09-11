#pragma once
#include <windows.h>
#include <cstdint>
// Included by API.cpp, No constructs allowed here


struct InstanceData;
typedef bool (*PfnCoreInitialize)(InstanceData*);
typedef bool (*PfnCoreDrawCircle)(InstanceData, int, int, int);
typedef bool (*PfnCoreDrawRectangle)(InstanceData, int, int, int, int);
typedef bool (*PfnCoreInitRenderWindow)(InstanceData, int64_t);
typedef bool (*PfnCoreUpdateRenderWindow)(InstanceData, float);
typedef bool (*PfnCoreDisposeRenderWindow)(InstanceData);
typedef bool (*PfnTemp_IsRenderWindowOpen)(InstanceData);
typedef bool (*PfnCoreDispose)(InstanceData*);

struct InstanceData
{
    HMODULE hDll;

    PfnCoreInitialize pfnCoreInitialize;
    PfnCoreDrawCircle pfnCoreDrawCircle;
    PfnCoreDrawRectangle pfnCoreDrawRectangle;
    PfnCoreInitRenderWindow pfnCoreInitRenderWindow;
    PfnCoreUpdateRenderWindow pfnCoreUpdateRenderWindow;
    PfnCoreDisposeRenderWindow pfnCoreDisposeRenderWindow;
    PfnTemp_IsRenderWindowOpen pfnTemp_IsRenderWindowOpen;
    PfnCoreDispose pfnCoreDispose;

    void* pCoreData;
    char* pPublicStatusMessage;
};