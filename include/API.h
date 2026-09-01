#pragma once
#include <stdio.h>
#include <windows.h>

struct HImageWriterInstance
{
    void* pData;
};
extern "C" __declspec(dllexport) void CreateImageWriterInstance(HImageWriterInstance* pInstance);
extern "C" __declspec(dllexport) void SetupImage(HImageWriterInstance instance, int width, int height);
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
typedef void (*PfnCoreExportImage)(InstanceData);
typedef void (*PfnCoreDispose)(InstanceData*);

struct InstanceData
{
    HMODULE hDll;

    PfnCoreInitialize pfnCoreInitialize;
    PfnCoreSetupImage pfnCoreSetupImage;
    PfnCoreExportImage pfnCoreExportImage;
    PfnCoreDispose pfnCoreDispose;
    void* pCoreData;
};