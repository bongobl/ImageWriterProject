#include <ImageWriter/API.h>
#include <ImageWriter/CoreAndAPIShared.h>

// Note: It is important that this file remains free of any C++ constructs or else GNU won't
// be able to build it into a dll that python can load

extern "C" __declspec(dllexport) bool CreateImageWriterInstance(HImageWriterInstance* pInstance)
{
    if(!pInstance){
        fprintf(stderr, "CreateImageWriterInstance: pInstance was null\n");
        return false;
    }
    
    pInstance->pData = (InstanceData*)malloc(sizeof(InstanceData));
    InstanceData* pInstanceData = (InstanceData*)pInstance->pData;

    HMODULE hDll = LoadLibrary("ImageWriterCore");

    if(!hDll){
        fprintf(stderr, "CreateImageWriterInstance: ImageWriterCore library could not load\n");
        return false;
    }
    
    PfnCoreInitialize pfnCoreInitialize = (PfnCoreInitialize)GetProcAddress(hDll, "initialize");

    if(pfnCoreInitialize == NULL){
        fprintf(stderr, "CreateImageWriterInstance: CoreEntry function initialize could not load\n");
        FreeLibrary(hDll);
        return false;
    }

    PfnCoreDrawCircle pfnCoreDrawCircle = (PfnCoreDrawCircle)GetProcAddress(hDll, "drawCircle");

    if (pfnCoreDrawCircle == NULL) {
        fprintf(stderr, "CreateImageWriterInstance: CoreEntry function drawCircle could not load\n");
        FreeLibrary(hDll);
        return false;
    }

    PfnCoreDrawRectangle pfnCoreDrawRectangle = (PfnCoreDrawRectangle)GetProcAddress(hDll, "drawRectangle");

    if (pfnCoreDrawRectangle == NULL) {
        fprintf(stderr, "CreateImageWriterInstance: CoreEntry function drawRectangle could not load\n");
        FreeLibrary(hDll);
        return false;
    }

    PfnCoreClearImage pfnCoreClearImage = (PfnCoreClearImage)GetProcAddress(hDll, "clearImage");

    if (pfnCoreClearImage == NULL) {
        fprintf(stderr, "CreateImageWriterInstance: CoreEntry function clearImage could not load\n");
        FreeLibrary(hDll);
        return false;
    }

    PfnCoreInitRenderWindow pfnCoreInitRenderWindow = (PfnCoreInitRenderWindow)GetProcAddress(hDll, "initRenderWindow");

    if (pfnCoreInitRenderWindow == NULL) {
        fprintf(stderr, "CreateImageWriterInstance: CoreEntry function initRenderWindow could not load\n");
        FreeLibrary(hDll);
        return false;
    }

    PfnCoreUpdateRenderWindow pfnCoreUpdateRenderWindow = (PfnCoreUpdateRenderWindow)GetProcAddress(hDll, "updateRenderWindow");

    if (pfnCoreUpdateRenderWindow == NULL) {
        fprintf(stderr, "CreateImageWriterInstance: CoreEntry function updateRenderWindow could not load\n");
        FreeLibrary(hDll);
        return false;
    }

    PfnCoreDisposeRenderWindow pfnCoreDisposeRenderWindow = (PfnCoreDisposeRenderWindow)GetProcAddress(hDll, "disposeRenderWindow");

    if (pfnCoreDisposeRenderWindow == NULL) {
        fprintf(stderr, "CreateImageWriterInstance: CoreEntry function disposeRenderWindow could not load\n");
        FreeLibrary(hDll);
        return false;
    }

    PfnIsIsolatedRenderWindowOpen pfnIsIsolatedRenderWindowOpen = (PfnIsIsolatedRenderWindowOpen)GetProcAddress(hDll, "isIsolatedRenderWindowOpen");

    if (pfnIsIsolatedRenderWindowOpen == NULL) {
        fprintf(stderr, "CreateImageWriterInstance: CoreEntry function isIsolatedRenderWindowOpen could not load\n");
        FreeLibrary(hDll);
        return false;
    }


    PfnCoreDispose pfnCoreDispose = (PfnCoreDispose)GetProcAddress(hDll, "dispose");

    if (pfnCoreDispose == NULL) {
        fprintf(stderr, "CreateImageWriterInstance: CoreEntry function dispose could not load\n");
        FreeLibrary(hDll);
        return false;
    }


    *pInstanceData = {
        .hDll = hDll,
        .pfnCoreInitialize = pfnCoreInitialize,
        .pfnCoreDrawCircle = pfnCoreDrawCircle,
        .pfnCoreDrawRectangle = pfnCoreDrawRectangle,
        .pfnCoreClearImage = pfnCoreClearImage,
        .pfnCoreInitRenderWindow = pfnCoreInitRenderWindow,
        .pfnCoreUpdateRenderWindow = pfnCoreUpdateRenderWindow,
        .pfnCoreDisposeRenderWindow = pfnCoreDisposeRenderWindow,
        .pfnIsIsolatedRenderWindowOpen = pfnIsIsolatedRenderWindowOpen,
        .pfnCoreDispose = pfnCoreDispose,
    };

    // initialize core
    return pInstanceData->pfnCoreInitialize(pInstanceData);
}

extern "C" __declspec(dllexport) bool DrawCircle(HImageWriterInstance instance, char* pStatusMessage, int centerX, int centerY, int radius)
{
    if (!instance.pData) {
        fprintf(stderr, "DrawCircle: instance.pData was null\n");
        strcpy(pStatusMessage, "ImageWiter app did not call DrawCircle() from its underlying framework correctly");
        return false;
    }

    InstanceData* pInstanceData = (InstanceData*)instance.pData;
    pInstanceData->pPublicStatusMessage = pStatusMessage;
    return pInstanceData->pfnCoreDrawCircle(*pInstanceData, centerX, centerY, radius);
}

extern "C" __declspec(dllexport) bool DrawRectangle(HImageWriterInstance instance, char* pStatusMessage, int centerX, int centerY, int halfExtentX, int halfExtentY)
{
    if (!instance.pData) {
        fprintf(stderr, "DrawRectangle: instance.pData was null\n");
        strcpy(pStatusMessage, "ImageWiter app did not call DrawRectangle() from its underlying framework correctly");
        return false;
    }

    InstanceData* pInstanceData = (InstanceData*)instance.pData;
    pInstanceData->pPublicStatusMessage = pStatusMessage;
    return pInstanceData->pfnCoreDrawRectangle(*pInstanceData, centerX, centerY, halfExtentX, halfExtentY);
}

extern "C" __declspec(dllexport) bool ClearImage(HImageWriterInstance instance, char* pStatusMessage)
{
    if (!instance.pData) {
        fprintf(stderr, "ClearImage: instance.pData was null\n");
        strcpy(pStatusMessage, "ImageWiter app did not call ClearImage() from its underlying framework correctly");
        return false;
    }

    InstanceData* pInstanceData = (InstanceData*)instance.pData;
    pInstanceData->pPublicStatusMessage = pStatusMessage;
    return pInstanceData->pfnCoreClearImage(*pInstanceData);
}
extern "C" __declspec(dllexport) bool InitRenderWindow(HImageWriterInstance instance, int64_t windowHandle)
{
    if (!instance.pData) {
        fprintf(stderr, "InitRenderWindow: instance.pData was null\n");
        return false;
    }

    InstanceData* pInstanceData = (InstanceData*)instance.pData;

    return pInstanceData->pfnCoreInitRenderWindow(*pInstanceData, windowHandle);
}

extern "C" __declspec(dllexport) bool UpdateRenderWindow(HImageWriterInstance instance, float deltaTime)
{
    if (!instance.pData) {
        fprintf(stderr, "UpdateRenderWindow: instance.pData was null\n");
        return false;
    }

    InstanceData* pInstanceData = (InstanceData*)instance.pData;

    return pInstanceData->pfnCoreUpdateRenderWindow(*pInstanceData, deltaTime);
}

extern "C" __declspec(dllexport) bool DisposeRenderWindow(HImageWriterInstance instance)
{
    if (!instance.pData) {
        fprintf(stderr, "DisposeRenderWindow: instance.pData was null\n");
        return false;
    }

    InstanceData* pInstanceData = (InstanceData*)instance.pData;

    return pInstanceData->pfnCoreDisposeRenderWindow(*pInstanceData);
}

extern "C" __declspec(dllexport) bool IsIsolatedRenderWindowOpen(HImageWriterInstance instance)
{
    if (!instance.pData) {
        fprintf(stderr, "IsIsolatedRenderWindowOpen: instance.pData was null\n");
        return false;
    }

    InstanceData* pInstanceData = (InstanceData*)instance.pData;

    return pInstanceData->pfnIsIsolatedRenderWindowOpen(*pInstanceData);
}
extern "C" __declspec(dllexport) bool DestroyImageWriterInstance(HImageWriterInstance* instance)
{
	if (!instance || !instance->pData) {
		fprintf(stderr, "DestroyImageWriterInstance: instance or instance->pData was null\n");
		return false;
	}
	InstanceData* pInstanceData = (InstanceData*)instance->pData;

    bool coreDisposeResult = pInstanceData->pfnCoreDispose(pInstanceData);

	FreeLibrary(pInstanceData->hDll);

	free(instance->pData);
	instance->pData = nullptr;

    return coreDisposeResult;
}