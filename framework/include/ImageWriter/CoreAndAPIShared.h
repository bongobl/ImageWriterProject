#pragma once
#include <windows.h>

// Included by API.cpp, No constructs allowed here


struct InstanceData;
typedef bool (*PfnCoreInitialize)(InstanceData*);
typedef bool (*PfnCoreSetupImage)(InstanceData, int, int);
typedef bool (*PfnCoreDrawCircle)(InstanceData, int, int, int);
typedef bool (*PfnCoreDrawRectangle)(InstanceData, int, int, int, int);
typedef bool (*PfnCoreExportImage)(InstanceData, const char*);
typedef bool (*PfnCoreTemp_RunSFMLWindow)(InstanceData);
typedef bool (*PfnCoreDispose)(InstanceData*);

struct InstanceData
{
    HMODULE hDll;

    PfnCoreInitialize pfnCoreInitialize;
    PfnCoreSetupImage pfnCoreSetupImage;
    PfnCoreDrawCircle pfnCoreDrawCircle;
    PfnCoreDrawRectangle pfnCoreDrawRectangle;
    PfnCoreExportImage pfnCoreExportImage;
    PfnCoreTemp_RunSFMLWindow pfnCoreTemp_RunSFMLWindow;
    PfnCoreDispose pfnCoreDispose;

    void* pCoreData;
    char* pPublicStatusMessage;
};