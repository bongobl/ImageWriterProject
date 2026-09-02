#include <Image.h>
#include <iostream>
#include <CoreEntry.h>


// TODO: move to core private header
struct CoreData {
	Image image;
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

	Image& image = pCoreData->image;

	if (image.getMode() != Image::NONE) {
		image.dispose();
	}
	image.setAsWrite(width, height);

	image.setSolidColor(Pixel(0, 0, 0));
}

extern "C" __declspec(dllexport) void drawCircle(InstanceData instanceData, int centerX, int centerY, int radius)
{
	if (!instanceData.pCoreData) {
		std::cerr << "setupImage: instanceData.pCoreData was null" << std::endl;
	}

	CoreData* pCoreData = static_cast<CoreData*>(instanceData.pCoreData);

	Image& image = pCoreData->image;

	for (int y = 0; y < image.getHeight(); ++y) {
		for (int x = 0; x < image.getWidth(); ++x) {

			int deltaX = x - centerX;
			int deltaY = y - centerY;

			int distSquared = deltaX * deltaX + deltaY * deltaY;
			int radiusSquared = radius *radius;

			if (distSquared <= radiusSquared) {
				image.setPixel(x, y, Pixel(0, 255, 0));
			}
		}
	}
}

extern "C" __declspec(dllexport) void drawRectangle(InstanceData instanceData, int centerX, int centerY, int halfExtentX, int halfExtentY)
{
	if (!instanceData.pCoreData) {
		std::cerr << "setupImage: instanceData.pCoreData was null" << std::endl;
	}

	CoreData* pCoreData = static_cast<CoreData*>(instanceData.pCoreData);

	Image& image = pCoreData->image;

	for (int y = 0; y < image.getHeight(); ++y) {
		for (int x = 0; x < image.getWidth(); ++x) {

			int deltaX = abs(x - centerX);
			int deltaY = abs(y - centerY);


			if (deltaX <= halfExtentX && deltaY <= halfExtentY) {
				image.setPixel(x, y, Pixel(250, 0, 250));
			}
		}
	}
}

extern "C" __declspec(dllexport) void exportImage(InstanceData instanceData)
{
	if (!instanceData.pCoreData) {
		std::cerr << "exportImage: instanceData.pCoreData was null" << std::endl;
	}

	CoreData* pCoreData = static_cast<CoreData*>(instanceData.pCoreData);
	Image& dummyImage = pCoreData->image;

	dummyImage.exportPNG("DummyImage.png");
	system("DummyImage.png");
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