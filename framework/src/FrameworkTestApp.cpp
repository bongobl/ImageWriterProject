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
	Transform transform = { .positionX = cameraTransform.scaleX, .positionY = cameraTransform.scaleY, .scaleX = 1, .scaleY = 1, .angle = 0 };
	AddEllipse(instance, statusMessage, &transform);
	
	transform = { .positionX = -cameraTransform.scaleX, .positionY = -cameraTransform.scaleY, .scaleX = 3, .scaleY = 2, .angle = 20 };
	AddRectangle(instance, statusMessage, &transform);
	printf("%s\n", statusMessage);

	system("pause");
	ClearImage(instance, statusMessage);

	system("pause");
	transform = { .positionX = 0, .positionY = 0, .scaleX = cameraTransform.scaleX - 1, .scaleY = cameraTransform.scaleY - 1, .angle = -30 };
	AddRectangle(instance, statusMessage, &transform);

	transform = { .positionX = 3, .positionY = -2, .scaleX = 3, .scaleY = 2, .angle = -10 };
	AddEllipse(instance, statusMessage, &transform);
	

	system("pause");
	ClearImage(instance, statusMessage);

	system("pause");
	transform = { .positionX = -5, .positionY = 3, .scaleX = 1, .scaleY = 4, .angle = -25 };
	AddEllipse(instance, statusMessage, &transform);

	transform = { .positionX = 7, .positionY = -4, .scaleX = 0.5f, .scaleY = 1.5f, .angle = 60 };
	AddEllipse(instance, statusMessage, &transform);

	transform = { .positionX = -8, .positionY = -1, .scaleX = 1, .scaleY = 1, .angle = 12 };
	AddRectangle(instance, statusMessage, &transform);

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