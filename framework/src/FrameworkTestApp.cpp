#include <ImageWriter/API.h>
#include <cstdlib>
#include <iostream>
#include <thread>
#include <chrono>

void runImageWriterFlow(HImageWriterInstance instance)
{
	char statusMessage[256] = "Command executed successfully";

	Transform cameraTransform = {};

	GetCameraTransform(instance, statusMessage, &cameraTransform);
	std::cout << "scaleX = " << cameraTransform.scaleX << ", scaleY = " << cameraTransform.scaleY << std::endl;

	system("pause");
	AddEllipse(instance, statusMessage, 
		{ .positionX = cameraTransform.scaleX, .positionY = cameraTransform.scaleY, .scaleX = 1, .scaleY = 1, .angle = 0 },
		{.red = 1, .green = 0, .blue = 0, .alpha = 1});
	
	AddRectangle(instance, statusMessage, 
		{ .positionX = -cameraTransform.scaleX, .positionY = -cameraTransform.scaleY, .scaleX = 3, .scaleY = 2, .angle = 20 }, 
		{ .red = 0.5f, .green = 0.5f, .blue = 1, .alpha = 1 });

	AddTriangle(instance, statusMessage,
		{ .positionX = 0, .positionY = 0, .scaleX = 1, .scaleY = 1, .angle = 60 },
		{ .red = 0, .green = 1, .blue = 0, .alpha = 1 },
		{ 1, -1 }, { 0,2 }, { -1,-1 });

	printf("%s\n", statusMessage);

	system("pause");
	ClearImage(instance, statusMessage);

	system("pause");
	AddRectangle(instance, statusMessage, 
		{ .positionX = 0, .positionY = 0, .scaleX = cameraTransform.scaleX - 1, .scaleY = cameraTransform.scaleY - 1, .angle = -30 }, 
		{ .red = 0.5f, .green = 1, .blue = 0.5f, .alpha = 0.35f });

	AddTriangle(instance, statusMessage,
		{ .positionX = 0, .positionY = 0, .scaleX = 1, .scaleY = 1, .angle = 0 },
		{ .red = 1, .green = 1, .blue = 0, .alpha = 1 },
		{ 3,1 }, { 1,3 }, { -2,0 });

	AddEllipse(instance, statusMessage, 
		{ .positionX = 3, .positionY = -2, .scaleX = 3, .scaleY = 2, .angle = -10 }, 
		{ .red = 0, .green = 0, .blue = 1, .alpha = 0.4f });
	

	system("pause");
	ClearImage(instance, statusMessage);

	system("pause");
	AddEllipse(instance, statusMessage, 
		{ .positionX = -5, .positionY = 3, .scaleX = 1, .scaleY = 4, .angle = -25 }, 
		{ .red = 0, .green = 1, .blue = 1, .alpha = 0.6f });

	AddEllipse(instance, statusMessage, 
		{ .positionX = 7, .positionY = -4, .scaleX = 0.5f, .scaleY = 1.5f, .angle = 60 }, 
		{ .red = 1, .green = 1, .blue = 0, .alpha = 0.2f });

	AddRectangle(instance, statusMessage, 
		{ .positionX = -8, .positionY = -1, .scaleX = 1, .scaleY = 1, .angle = 12 }, 
		{ .red = 1, .green = 0.5, .blue = 1, .alpha = 0.6f });

	system("pause");
	ClearImage(instance, statusMessage);
}
int main(void)
{
	HImageWriterInstance instance = {};

	if (!CreateImageWriterInstance(&instance)) {
		return EXIT_FAILURE;
	}

	// Create an isolated window with a handle value of 0
	int windowHwnd = 0;
	InitRenderWindow(instance, windowHwnd);

	std::thread imageWriterThread(runImageWriterFlow, instance);


	auto prevTime = std::chrono::steady_clock::now();

	while (IsIsolatedRenderWindowOpen(instance)) {
		auto currentTime = std::chrono::steady_clock::now();

		std::chrono::duration<float> elapsed = currentTime - prevTime;

		float deltaTime = elapsed.count();

		std::this_thread::sleep_for(std::chrono::milliseconds(16));

		UpdateRenderWindow(instance, deltaTime);

		prevTime = currentTime;
	}

	DisposeRenderWindow(instance);

	if (imageWriterThread.joinable()) {
		imageWriterThread.join();
	}
	DestroyImageWriterInstance(&instance);
	
	
	return EXIT_SUCCESS;
}