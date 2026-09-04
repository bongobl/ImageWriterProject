#include <API.h>
#include <cstdlib>

int main(void)
{
	HImageWriterInstance instance = {};

	if (!CreateImageWriterInstance(&instance)) {
		return EXIT_FAILURE;
	}

	system("pause");
	SetupImage(instance, 1920, 1080);
	DrawCircle(instance, 350, 200, 100);
	DrawRectangle(instance, 1500, 700, 200, 150);
	ExportImage(instance, "ImageLarge");

	system("pause");
	SetupImage(instance, 640, 480);
	DrawCircle(instance, 400, 200, 80);
	DrawRectangle(instance, 100, 150, 75, 120);
	ExportImage(instance, "ImageSmall");

	system("pause");
	SetupImage(instance, 1280, 720);
	DrawCircle(instance, 800, 300, 70);
	DrawCircle(instance, 300, 550, 150);
	DrawRectangle(instance, 500, 300, 400, 10);
	ExportImage(instance, "ImageMedium");

	DestroyImageWriterInstance(&instance);
	return EXIT_SUCCESS;
}