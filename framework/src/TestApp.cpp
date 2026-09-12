#include <ImageWriter/API.h>
#include <cstdlib>
#include <iostream>
#include <thread>
#include <chrono>

void runImageWriterFlow(HImageWriterInstance instance)
{
	char statusMessage[256] = "Command executed successfully";

	system("pause");
	DrawCircle(instance, statusMessage, 350, 200, 100);
	DrawRectangle(instance, statusMessage, 1500, 700, 200, 150);
	printf("%s\n", statusMessage);

	system("pause");
	DrawCircle(instance, statusMessage, 400, 200, 80);
	DrawRectangle(instance, statusMessage, 100, 150, 75, 120);

	system("pause");
	DrawCircle(instance, statusMessage, 800, 300, 70);
	DrawCircle(instance, statusMessage, 300, 550, 150);
	DrawRectangle(instance, statusMessage, 500, 300, 400, 10);
}
int main(void)
{
	HImageWriterInstance instance = {};

	if (!CreateImageWriterInstance(&instance)) {
		return EXIT_FAILURE;
	}

	std::thread imageWriterThread(runImageWriterFlow, instance);


	auto prevTime = std::chrono::steady_clock::now();

	// Create an isolated window with a handle value of 0
	int windowHwnd = 0;
	InitRenderWindow(instance, windowHwnd);
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