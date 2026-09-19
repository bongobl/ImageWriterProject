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
	DrawCircle(instance, statusMessage, cameraTransform.scaleX, cameraTransform.scaleY, 1);
	
	Transform transform = { .positionX = -cameraTransform.scaleX, .positionY = -cameraTransform.scaleY, .scaleX = 3, .scaleY = 2, .angle = 20 };
	AddRectangle(instance, statusMessage, &transform);
	printf("%s\n", statusMessage);

	system("pause");
	ClearImage(instance, statusMessage);

	system("pause");
	transform = { .positionX = 0, .positionY = 0, .scaleX = cameraTransform.scaleX - 1, .scaleY = cameraTransform.scaleY - 1, .angle = -30 };
	AddRectangle(instance, statusMessage, &transform);
	DrawCircle(instance, statusMessage, 3, -2, 3);
	

	system("pause");
	ClearImage(instance, statusMessage);

	system("pause");
	DrawCircle(instance, statusMessage, -5, 3, 2);
	DrawCircle(instance, statusMessage, 7, -4, 0.5f);

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