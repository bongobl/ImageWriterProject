#pragma once
#include <stdio.h>
#include <windows.h>

struct HImageWriterInstance
{
    void* pData;
};
extern "C" __declspec(dllexport) void CreateImageWriterInstance(HImageWriterInstance* pInstance);
extern "C" __declspec(dllexport) void SetupImage(HImageWriterInstance instance, int width, int height);
extern "C" __declspec(dllexport) void DrawCircle(HImageWriterInstance instance, int centerX, int centerY, int radius);
extern "C" __declspec(dllexport) void DrawRectangle(HImageWriterInstance instance, int centerX, int centerY, int halfExtentX, int halfExtentY);
extern "C" __declspec(dllexport) void ExportImage(HImageWriterInstance instance);
extern "C" __declspec(dllexport) void DestroyImageWriterInstance(HImageWriterInstance* instance);

// Private
// TODO: 
// - move to private header shared between API and core
// - have API.cpp and CoreEntry.h include private header
// - remember to keep C++ constructs out of it

struct InstanceData;
typedef void (*PfnCoreInitialize)(InstanceData*);
typedef void (*PfnCoreSetupImage)(InstanceData, int, int);
typedef void (*PfnCoreDrawCircle)(InstanceData, int, int, int);
typedef void (*PfnCoreDrawRectangle)(InstanceData, int, int, int, int);
typedef void (*PfnCoreExportImage)(InstanceData);
typedef void (*PfnCoreDispose)(InstanceData*);

struct InstanceData
{
    HMODULE hDll;

    PfnCoreInitialize pfnCoreInitialize;
    PfnCoreSetupImage pfnCoreSetupImage;
    PfnCoreDrawCircle pfnCoreDrawCircle;
    PfnCoreDrawRectangle pfnCoreDrawRectangle;
    PfnCoreExportImage pfnCoreExportImage;
    PfnCoreDispose pfnCoreDispose;
    void* pCoreData;
};