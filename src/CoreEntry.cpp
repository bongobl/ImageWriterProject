#include <iostream>
#include <CoreEntry.h>
#include <stdio.h>

extern "C" __declspec(dllexport) void generateImage(int width, int height)
{
	//printf("called this C function\n");
	Image dummyImage;
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
	dummyImage.exportPNG("C:\\Dev\\Practice\\BasicClaude\\ImageWriter\\buildGNU\\DummyImage.png");
	dummyImage.dispose();
	system("C:\\Dev\\Practice\\BasicClaude\\ImageWriter\\buildGNU\\DummyImage.png");
}