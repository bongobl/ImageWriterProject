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
	DrawRectangle(instance, statusMessage, -cameraTransform.scaleX, -cameraTransform.scaleY, 3, 2);
	printf("%s\n", statusMessage);

	system("pause");
	ClearImage(instance, statusMessage);

	system("pause");
	DrawRectangle(instance, statusMessage, 0, 0, cameraTransform.scaleX - 1, cameraTransform.scaleY - 1);
	DrawCircle(instance, statusMessage, 3, -2, 3);
	

	system("pause");
	ClearImage(instance, statusMessage);

	system("pause");
	DrawCircle(instance, statusMessage, -5, 3, 2);
	DrawCircle(instance, statusMessage, 7, -4, 0.5f);
	DrawRectangle(instance, statusMessage, -8, -1, 1, 3);

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