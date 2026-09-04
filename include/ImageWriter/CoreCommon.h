#pragma once
#include <ImageWriter/Image.h>

// C++ constructs allowed here

struct CoreData {
	Image image;
	bool setupCalled = false; // needs to be called at least once before we can draw to image
};