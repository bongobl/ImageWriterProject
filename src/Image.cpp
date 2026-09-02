#include <vector>
#include <Image.h>

// For reading images
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#endif

// For writing images
#ifndef STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#endif

template <typename T>
static T clamp(T value, T min, T max) {
	if (value < min) {
		return min;
	}
	else if (value > max) {
		return max;
	}
	return value;
}

Pixel::Pixel(int r, int g, int b) {
	red = clamp(r, 0, 255);
	green = clamp(g, 0, 255);
	blue = clamp(b, 0, 255);
}

Image::Image(){
	mode = NONE;
	width = height = 0;
	pixels = NULL;
	
}

Image::~Image(){
	dispose();
}

void Image::dispose() {
	if (pixels != NULL) {
		delete[] pixels;
		pixels = NULL;
	}
	mode = NONE;
	width = height = 0;
}

int Image::getWidth()
{
	return width;
}
int Image::getHeight()
{
	return height;
}
void Image::setAsRead(std::string inFileName){

	if(mode != NONE){
		std::cerr << "Image: Image mode already set, can't be changed" << std::endl;
		return;
	}
	mode = READ;

	//read in the file here
	int numChannels = -1;
	unsigned char* data;
	data = stbi_load(inFileName.c_str(), &width, &height, &numChannels, STBI_rgb);
	if (numChannels == -1) {
		std::cerr << "Image: Image name " << inFileName << " not found" << std::endl;
		return;
	}
	pixels = new Pixel[height * width];
	
	//parse here
	for(int i = 0; i < width * height; ++i){ 
		Pixel* currPixel = &pixels[i];

		currPixel->red = data[i * 3 + 0];
		currPixel->green = data[i * 3 + 1];
		currPixel->blue = data[i * 3 + 2];

	}
	stbi_image_free(data);

}
void Image::setAsWrite(int image_width, int image_height){
	if(mode != NONE){
		std::cerr << "Image: Image mode and resolution already set, can't be changed" << std::endl;
		return;
	}
	mode = WRITE;
	width = image_width;
	height = image_height;
	pixels = new Pixel[height * width];
}

void Image::setSolidColor(Pixel color) {
	if (mode != WRITE) {
		std::cerr << "Image: Image must be in WRITE mode to set Solid Color" << std::endl;
		return;
	}
	for (int pixelY = 0; pixelY < height; ++pixelY) {
		for (int pixelX = 0; pixelX < width; ++pixelX) {
			setPixel(pixelX, pixelY, color);
		}
	}
}

void Image::setPixel(int x_coord, int y_coord, Pixel color){
	
	if(mode != WRITE){
		std::cerr << "Image: Image must be in WRITE mode to set pixel" << std::endl;
		return;
	}
	//DO PARAMETER ERROR CHECKING HERE
	if(x_coord >= width || y_coord >= height || x_coord < 0 || y_coord < 0){
		return;
	}
	
	//perform function
	pixels[y_coord * width + x_coord] = color;


}
void Image::exportPNG(std::string image_name) const{

	//error check
	if(mode != WRITE){
		std::cerr << "Image: Image must be in WRITE mode to exportPPM" << std::endl;
		return;
	}

	//export png
	std::vector<unsigned char> data;
    for(int i = 0; i < width * height; ++i){ 
		Pixel currPixel = pixels[i];
		data.push_back((unsigned char)currPixel.red);
		data.push_back((unsigned char)currPixel.green);
		data.push_back((unsigned char)currPixel.blue);
	}
    stbi_write_png(image_name.c_str(), width, height, 3, data.data(), width * 3);
}

int Image::getMode() const{
	return mode;
}

Pixel Image::sample(float x, float y) const {


	//return plain white color if image pixels don't exist
	if(mode == NONE){
		return Pixel(255, 255, 255);
	}

	
	//convert to pixel coordinates
	int x_coord = (int)(x * width);
	int y_coord = (int)((1 - y) * height);

	
	//bounds checking (can't use setPixel if in read mode)
	if (x_coord >= width)
		x_coord = width - 1;
	if (y_coord >= height)
		y_coord = height - 1;
	if (x_coord < 0)
		x_coord = 0;
	if (y_coord < 0)
		y_coord = 0;
	

	//sample image and return color
	Pixel sampledPixel = pixels[y_coord * width + x_coord];
	sampledPixel.red = clamp(sampledPixel.red, 0, 255);
	sampledPixel.green = clamp(sampledPixel.green, 0, 255);	
	sampledPixel.blue = clamp(sampledPixel.blue, 0, 255);
	return sampledPixel;

}