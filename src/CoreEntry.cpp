#include <Image.h>
#include <iostream>
#include <CoreEntry.h>


// TODO: move to core private header
struct CoreData {
	Image dummyImage;
};

extern "C" __declspec(dllexport) void initialize(InstanceData* pInstanceData)
{
	if (!pInstanceData) {
		std::cerr << "initialize: pInstanceData was null" << std::endl;
	}
	pInstanceData->pCoreData = new CoreData();
}
extern "C" __declspec(dllexport) void setupImage(InstanceData instanceData, int width, int height)
{
	if (!instanceData.pCoreData) {
		std::cerr << "setupImage: instanceData.pCoreData was null" << std::endl;
	}

	CoreData* pCoreData = static_cast<CoreData*>(instanceData.pCoreData);
	Image& dummyImage = pCoreData->dummyImage;

	dummyImage.setAsWrite(width, height);

	for(int y = 0; y < height; ++y){
		for(int x = 0; x < width; ++x){

			float r = (float)x / width;
			float d = (float)y / height;
			float l = 1 - r;
			float u = 1 - d;

			float red = d;
			float green = (d * l) +(u * r);
			float blue = u;
			dummyImage.setPixel(x, y, Pixel(red * 255.0f,green * 255.0f,blue * 255.0f));
		}
	}
}

extern "C" __declspec(dllexport) void exportImage(InstanceData instanceData)
{
	if (!instanceData.pCoreData) {
		std::cerr << "exportImage: instanceData.pCoreData was null" << std::endl;
	}

	CoreData* pCoreData = static_cast<CoreData*>(instanceData.pCoreData);
	Image& dummyImage = pCoreData->dummyImage;

	dummyImage.exportPNG("C:\\Dev\\Practice\\BasicClaude\\ImageWriter\\buildGNU\\DummyImage.png");
	dummyImage.dispose();
	system("C:\\Dev\\Practice\\BasicClaude\\ImageWriter\\buildGNU\\DummyImage.png");
}

extern "C" __declspec(dllexport) void dispose(InstanceData* pInstanceData)
{
	if (!pInstanceData) {
		std::cerr << "dispose: pInstanceData was null" << std::endl;
	}

	if (!pInstanceData->pCoreData) {
		std::cerr << "dispose: pInstanceData->pCoreData was null" << std::endl;
	}

	delete static_cast<CoreData*>(pInstanceData->pCoreData);
	pInstanceData->pCoreData = nullptr;
}