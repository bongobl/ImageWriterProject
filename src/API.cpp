#include <API.h>
#include <windows.h>

typedef void (*PfnGenerateImage)(int, int);

extern "C" __declspec(dllexport) void GenImage(int width, int height)
{
    HMODULE hDll = LoadLibrary("C:\\Dev\\Practice\\BasicClaude\\ImageWriter\\buildGNU\\ImageWriterCore");

    PfnGenerateImage fnGenerateImage = (PfnGenerateImage)GetProcAddress(hDll, "generateImage");

    if(fnGenerateImage == NULL){
        FreeLibrary(hDll);
        return;
    }
    fnGenerateImage(width, height);

    FreeLibrary(hDll);
}