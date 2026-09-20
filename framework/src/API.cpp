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

    PfnCoreGetCameraTransform pfnCoreGetCameraTransform = (PfnCoreGetCameraTransform)GetProcAddress(hDll, "getCameraTransform");

    if (pfnCoreGetCameraTransform == NULL) {
        fprintf(stderr, "CreateImageWriterInstance: CoreEntry function getCameraTransform could not load\n");
        FreeLibrary(hDll);
        return false;
    }
    

    PfnCoreAddEllipse pfnCoreAddEllipse = (PfnCoreAddEllipse)GetProcAddress(hDll, "addEllipse");

    if (pfnCoreAddEllipse == NULL) {
        fprintf(stderr, "CreateImageWriterInstance: CoreEntry function addEllipse could not load\n");
        FreeLibrary(hDll);
        return false;
    }

    PfnCoreAddRectangle pfnCoreAddRectangle = (PfnCoreAddRectangle)GetProcAddress(hDll, "addRectangle");

    if (pfnCoreAddRectangle == NULL) {
        fprintf(stderr, "CreateImageWriterInstance: CoreEntry function addRectangle could not load\n");
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
        .pfnCoreGetCameraTransform = pfnCoreGetCameraTransform,
        .pfnCoreAddEllipse = pfnCoreAddEllipse,
        .pfnCoreAddRectangle = pfnCoreAddRectangle,
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

extern "C" __declspec(dllexport) bool GetCameraTransform(HImageWriterInstance instance, char* pStatusMessage, Transform* pCameraTransform)
{
    if (!instance.pData) {
        fprintf(stderr, "GetCameraTransform: instance.pData was null\n");
        strcpy(pStatusMessage, "ImageWiter app did not call GetCameraTransform() from its underlying framework correctly");
        return false;
    }

    InstanceData* pInstanceData = (InstanceData*)instance.pData;
    pInstanceData->pPublicStatusMessage = pStatusMessage;
    return pInstanceData->pfnCoreGetCameraTransform(*pInstanceData, pCameraTransform);
}

extern "C" __declspec(dllexport) bool AddEllipse(HImageWriterInstance instance, char* pStatusMessage, Transform transform, Color color)
{
    if (!instance.pData) {
        fprintf(stderr, "AddEllipse: instance.pData was null\n");
        strcpy(pStatusMessage, "ImageWiter app did not call AddEllipse() from its underlying framework correctly");
        return false;
    }

    InstanceData* pInstanceData = (InstanceData*)instance.pData;
    pInstanceData->pPublicStatusMessage = pStatusMessage;
    return pInstanceData->pfnCoreAddEllipse(*pInstanceData, transform, color);
}

extern "C" __declspec(dllexport) bool AddRectangle(HImageWriterInstance instance, char* pStatusMessage, Transform transform, Color color)
{
    if (!instance.pData) {
        fprintf(stderr, "AddRectangle: instance.pData was null\n");
        strcpy(pStatusMessage, "ImageWiter app did not call AddRectangle() from its underlying framework correctly");
        return false;
    }

    InstanceData* pInstanceData = (InstanceData*)instance.pData;
    pInstanceData->pPublicStatusMessage = pStatusMessage;
    return pInstanceData->pfnCoreAddRectangle(*pInstanceData, transform, color);
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