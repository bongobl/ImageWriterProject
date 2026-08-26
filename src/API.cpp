#include <iostream>
#include <Image.h>

extern "C" __declspec(dllexport) void generateImage()
{
	Image dummyImage;
	dummyImage.setAsWrite(1920, 1080);

	for(int y = 0; y < 1080; ++y){
		for(int x = 0; x < 1920; ++x){

			float r = (float)x / 1920.0f;
			float d = (float)y / 1080.0f;
			float l = 1 - r;
			float u = 1 - d;

			float red = d;
			float green = (d * l) +(u * r);
			float blue = u;
			dummyImage.setPixel(x, y, Pixel(red * 255.0f,green * 255.0f,blue * 255.0f));
		}
	}
	dummyImage.exportPNG("DummyImage.png");
	dummyImage.dispose();
	system("DummyImage.png");
}