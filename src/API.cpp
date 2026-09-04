#include <API.h>


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

    PfnCoreSetupImage pfnCoreSetupImage = (PfnCoreSetupImage)GetProcAddress(hDll, "setupImage");

    if (pfnCoreSetupImage == NULL) {
        fprintf(stderr, "CreateImageWriterInstance: CoreEntry function setupImage could not load\n");
        FreeLibrary(hDll);
        return false;
    }

    PfnCoreDrawCircle pfnCoreDrawCircle = (PfnCoreDrawCircle)GetProcAddress(hDll, "drawCircle");

    if (pfnCoreDrawCircle == NULL) {
        fprintf(stderr, "CreateImageWriterInstance: CoreEntry function pfnCoreDrawCircle could not load\n");
        FreeLibrary(hDll);
        return false;
    }

    PfnCoreDrawRectangle pfnCoreDrawRectangle = (PfnCoreDrawRectangle)GetProcAddress(hDll, "drawRectangle");

    if (pfnCoreDrawRectangle == NULL) {
        fprintf(stderr, "CreateImageWriterInstance: CoreEntry function pfnCoreDrawRectangle could not load\n");
        FreeLibrary(hDll);
        return false;
    }

    PfnCoreExportImage pfnCoreExportImage = (PfnCoreExportImage)GetProcAddress(hDll, "exportImage");

    if (pfnCoreExportImage == NULL) {
        fprintf(stderr, "CreateImageWriterInstance: CoreEntry function exportImage could not load\n");
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
        .pfnCoreSetupImage = pfnCoreSetupImage,
        .pfnCoreDrawCircle = pfnCoreDrawCircle,
        .pfnCoreDrawRectangle = pfnCoreDrawRectangle,
        .pfnCoreExportImage = pfnCoreExportImage,
        .pfnCoreDispose = pfnCoreDispose,
    };

    // initialize core
    return pInstanceData->pfnCoreInitialize(pInstanceData);
}

extern "C" __declspec(dllexport) bool SetupImage(HImageWriterInstance instance, int width, int height)
{
	if (!instance.pData) {
		fprintf(stderr, "SetupImage: instance.pData was null\n");
		return false;
	}

    InstanceData* pInstanceData = (InstanceData*)instance.pData;
    return pInstanceData->pfnCoreSetupImage(*pInstanceData, width, height);
}

extern "C" __declspec(dllexport) bool DrawCircle(HImageWriterInstance instance, int centerX, int centerY, int radius)
{
    if (!instance.pData) {
        fprintf(stderr, "DrawCircle: instance.pData was null\n");
        return false;
    }

    InstanceData* pInstanceData = (InstanceData*)instance.pData;
    return pInstanceData->pfnCoreDrawCircle(*pInstanceData, centerX, centerY, radius);
}

extern "C" __declspec(dllexport) bool DrawRectangle(HImageWriterInstance instance, int centerX, int centerY, int halfExtentX, int halfExtentY)
{
    if (!instance.pData) {
        fprintf(stderr, "DrawCircle: instance.pData was null\n");
        return false;
    }

    InstanceData* pInstanceData = (InstanceData*)instance.pData;
    return pInstanceData->pfnCoreDrawRectangle(*pInstanceData, centerX, centerY, halfExtentX, halfExtentY);
}

extern "C" __declspec(dllexport) bool ExportImage(HImageWriterInstance instance, const char* pImageName)
{
    if (!instance.pData) {
        fprintf(stderr, "ExportImage: instance.pData was null\n");
        return false;
    }

    InstanceData* pInstanceData = (InstanceData*)instance.pData;
    return pInstanceData->pfnCoreExportImage(*pInstanceData, pImageName);
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