#include <ImageWriter/API.h>
#include <cstdlib>
#include <iostream>

int main(void)
{
	HImageWriterInstance instance = {};

	if (!CreateImageWriterInstance(&instance)) {
		return EXIT_FAILURE;
	}

	char statusMessage[256] = "Command executed successfully";

	system("pause");
	SetupImage(instance, statusMessage, 1920, 1080);
	DrawCircle(instance, statusMessage, 350, 200, 100);
	DrawRectangle(instance, statusMessage, 1500, 700, 200, 150);
	ExportImage(instance, statusMessage, "ImageLarge");
	printf("%s\n", statusMessage);

	system("pause");
	SetupImage(instance, statusMessage, 640, 480);
	DrawCircle(instance, statusMessage, 400, 200, 80);
	DrawRectangle(instance, statusMessage, 100, 150, 75, 120);
	ExportImage(instance, statusMessage, "ImageSmall");

	system("pause");
	SetupImage(instance, statusMessage, 1280, 720);
	DrawCircle(instance, statusMessage, 800, 300, 70);
	DrawCircle(instance, statusMessage, 300, 550, 150);
	DrawRectangle(instance, statusMessage, 500, 300, 400, 10);
	ExportImage(instance, statusMessage, "ImageMedium");

	TEMP_RunSFMLWindow(instance, statusMessage);

	DestroyImageWriterInstance(&instance);
	
	return EXIT_SUCCESS;
}