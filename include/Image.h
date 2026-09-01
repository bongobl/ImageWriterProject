#pragma once
#include <string>
#include <iostream>
#include <fstream>

//Used to store RGB values for image to be written
//Use Vec3 instead for all in-app purposes
struct Pixel {

	int red;
	int green;
	int blue;

	Pixel() {red = green = blue = 0;}
	Pixel(int r, int g, int b);
};

class Image {

	int width;
	int height;
	Pixel* pixels;
	int mode;
	

public:
	Image();
	~Image();
	void dispose();

	void setAsRead(std::string inFileName);
	void setAsWrite(int image_width, int image_height);
	
	void setSolidColor(Pixel color);
	void setPixel(int x_coord, int y_coord, Pixel color);
	void exportPNG(std::string image_name) const;
	int getMode() const;
	
	Pixel sample(float x, float y) const;


	enum IOMode {NONE, READ, WRITE};
};