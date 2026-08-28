#include <API.h>
#include <windows.h>

typedef void (*PfnGenerateImage)(int, int);

extern "C" __declspec(dllexport) void GenImage(int width, int height)
{
    HMODULE hDll = LoadLibrary("ImageWriterCore");

    PfnGenerateImage fnGenerateImage = (PfnGenerateImage)GetProcAddress(hDll, "generateImage");

    if(fnGenerateImage == NULL){
        fprintf(stderr, "Failed to load image writer core library\n");
        FreeLibrary(hDll);
        return;
    }
    fnGenerateImage(width, height);

    FreeLibrary(hDll);
}