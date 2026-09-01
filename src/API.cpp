#include <API.h>


extern "C" __declspec(dllexport) void CreateImageWriterInstance(HImageWriterInstance* pInstance)
{
    if(!pInstance){
        fprintf(stderr, "CreateImageWriterInstance: pInstance was null\n");
        return;
    }
    
    pInstance->pData = (InstanceData*)malloc(sizeof(InstanceData));
    InstanceData* pInstanceData = (InstanceData*)pInstance->pData;

    HMODULE hDll = LoadLibrary("C:\\Dev\\Practice\\BasicClaude\\ImageWriter\\buildGNU\\ImageWriterCore");

    PfnCoreInitialize pfnCoreInitialize = (PfnCoreInitialize)GetProcAddress(hDll, "initialize");

    if(pfnCoreInitialize == NULL){
        fprintf(stderr, "CreateImageWriterInstance: CoreEntry function initialize could not load\n");
        FreeLibrary(hDll);
        return;
    }

    PfnCoreSetupImage pfnCoreSetupImage = (PfnCoreSetupImage)GetProcAddress(hDll, "setupImage");

    if (pfnCoreSetupImage == NULL) {
        fprintf(stderr, "CreateImageWriterInstance: CoreEntry function setupImage could not load\n");
        FreeLibrary(hDll);
        return;
    }

    PfnCoreExportImage pfnCoreExportImage = (PfnCoreExportImage)GetProcAddress(hDll, "exportImage");

    if (pfnCoreExportImage == NULL) {
        fprintf(stderr, "CreateImageWriterInstance: CoreEntry function exportImage could not load\n");
        FreeLibrary(hDll);
        return;
    }

    PfnCoreDispose pfnCoreDispose = (PfnCoreDispose)GetProcAddress(hDll, "dispose");

    if (pfnCoreDispose == NULL) {
        fprintf(stderr, "CreateImageWriterInstance: CoreEntry function dispose could not load\n");
        FreeLibrary(hDll);
        return;
    }


    *pInstanceData = {
        .hDll = hDll,
        .pfnCoreInitialize = pfnCoreInitialize,
        .pfnCoreSetupImage = pfnCoreSetupImage,
        .pfnCoreExportImage = pfnCoreExportImage,
        .pfnCoreDispose = pfnCoreDispose,
    };

    // initialize core
    pInstanceData->pfnCoreInitialize(pInstanceData);
}

extern "C" __declspec(dllexport) void SetupImage(HImageWriterInstance instance, int width, int height)
{
	if (!instance.pData) {
		fprintf(stderr, "SetupImage: instance.pData was null\n");
		return;
	}

    InstanceData* pInstanceData = (InstanceData*)instance.pData;
    pInstanceData->pfnCoreSetupImage(*pInstanceData, width, height);
}

extern "C" __declspec(dllexport) void ExportImage(HImageWriterInstance instance)
{
    if (!instance.pData) {
        fprintf(stderr, "ExportImage: instance.pData was null\n");
        return;
    }

    InstanceData* pInstanceData = (InstanceData*)instance.pData;
    pInstanceData->pfnCoreExportImage(*pInstanceData);
}
extern "C" __declspec(dllexport) void DestroyImageWriterInstance(HImageWriterInstance* instance)
{
	if (!instance || !instance->pData) {
		fprintf(stderr, "DestroyImageWriterInstance: instance or instance->pData was null\n");
		return;
	}
	InstanceData* pInstanceData = (InstanceData*)instance->pData;

    pInstanceData->pfnCoreDispose(pInstanceData);

	FreeLibrary(pInstanceData->hDll);

	free(instance->pData);
	instance->pData = nullptr;
}