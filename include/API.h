#pragma once
#include <stdio.h>
#include <windows.h>

struct HImageWriterInstance
{
    void* pData;
};
extern "C" __declspec(dllexport) bool CreateImageWriterInstance(HImageWriterInstance* pInstance);
extern "C" __declspec(dllexport) bool SetupImage(HImageWriterInstance instance, int width, int height);
extern "C" __declspec(dllexport) bool DrawCircle(HImageWriterInstance instance, int centerX, int centerY, int radius);
extern "C" __declspec(dllexport) bool DrawRectangle(HImageWriterInstance instance, int centerX, int centerY, int halfExtentX, int halfExtentY);
extern "C" __declspec(dllexport) bool ExportImage(HImageWriterInstance instance);
extern "C" __declspec(dllexport) bool DestroyImageWriterInstance(HImageWriterInstance* instance);

// Private
// TODO: 
// - move to private header shared between API and core
// - have API.cpp and CoreEntry.h include private header
// - remember to keep C++ constructs out of it

struct InstanceData;
typedef bool (*PfnCoreInitialize)(InstanceData*);
typedef bool (*PfnCoreSetupImage)(InstanceData, int, int);
typedef bool (*PfnCoreDrawCircle)(InstanceData, int, int, int);
typedef bool (*PfnCoreDrawRectangle)(InstanceData, int, int, int, int);
typedef bool (*PfnCoreExportImage)(InstanceData);
typedef bool (*PfnCoreDispose)(InstanceData*);

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