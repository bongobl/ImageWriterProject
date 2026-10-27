#include <ImageWriter/API.h>
#include <cstdlib>
#include <iostream>
#include <thread>
#include <chrono>

void runImageWriterFlow(HImageWriterInstance instance)
{

	TransformReply transformReply;
	if (!ImageWriter_Scene_GetCameraTransform(instance, &transformReply))
	{
		std::cerr << "ImageWriter error: " << transformReply.message << std::endl;
	}
	else 
	{
		std::cout << "ImageWriter success: " << transformReply.message << std::endl;
		std::cout << "scaleX = " << transformReply.transform.scale * transformReply.cameraWidthFromHeight << ", scaleY = " << transformReply.transform.scale << std::endl;
	}
	
	float dimX = 8;
	float dimY = 6;
	float spacingX = 2.4f;
	float spacingY = 1.3f;

	float startX = (dimX - 1) / 2 * spacingX;
	float startY = (dimY - 1) / 2 * spacingY;

	AddEntityReply addEntityReply;

	// Ellipses
	system("pause");
	for (int i = 0; i < dimY; ++i) {
		for (int j = 0; j < dimX; ++j) {
			if (!ImageWriter_Scene_AddEntity(
				instance, 
				&addEntityReply,
				{
					.entityType = ENTITY_TYPE_ELLIPSE,
					.transform = {
						.position = {
							.x = j * spacingX - startX,
							.y = i * spacingY - startY
						},
						.orientation = 0,
						.scale = 1
					},
					.color = { .red = 1, .green = 0, .blue = 0, .alpha = 1 },
					.ellipse = {
						.halfExtents = { 0.5f, 0.35f }
					}
				})
			)
				
			{
				std::cerr << "ImageWriter error: " << addEntityReply.message << std::endl;
			}
		}
	}
	
	PlainReply plainReply;

	system("pause");
	if (!ImageWriter_Scene_RemoveEntity(instance, &plainReply, 26))
	{
		std::cerr << "ImageWriter error: " << plainReply.message << std::endl;
	}

	system("pause");
	if(!ImageWriter_Scene_RemoveAllEntities(instance, &plainReply))
	{
		std::cerr << "ImageWriter error: " << plainReply.message << std::endl;
	}

	// Rectangles
	system("pause");
	for (int i = 0; i < dimY; ++i) {
		for (int j = 0; j < dimX; ++j) {
			if (!ImageWriter_Scene_AddEntity(
				instance,
				&addEntityReply,
				{
					.entityType = ENTITY_TYPE_RECTANGLE,
					.transform = {
						.position = {
							.x = j * spacingX - startX,
							.y = i * spacingY - startY
						},
						.orientation = 0,
						.scale = 1
					},
					.color = { .red = 0, .green = 1, .blue = 1, .alpha = 1 },
					.rectangle = {
						.halfExtents = { 0.5f, 0.35f }
					}
				})
			)

			{
				std::cerr << "ImageWriter error: " << addEntityReply.message << std::endl;
			}
		}
	}

	system("pause");
	if (!ImageWriter_Scene_RemoveAllEntities(instance, &plainReply))
	{
		std::cerr << "ImageWriter error: " << plainReply.message << std::endl;
	}

	// Triangles
	system("pause");
	for (int i = 0; i < dimY; ++i) {
		for (int j = 0; j < dimX; ++j) {
			if (!ImageWriter_Scene_AddEntity(
				instance,
				&addEntityReply,
				{
					.entityType = ENTITY_TYPE_TRIANGLE,
					.transform = {
						.position = {
							.x = j * spacingX - startX,
							.y = i * spacingY - startY
						},
						.orientation = 0,
						.scale = 1
					},
					.color = { .red = 1, .green = 1, .blue = 0, .alpha = 1 },
					.triangle = {
						.point1 = { -0.5f, 0.35f },
						.point2 = { -0.5f, -0.35f },
						.point3 = { 0.5f, 0 }
					}
				})
			)

			{
				std::cerr << "ImageWriter error: " << addEntityReply.message << std::endl;
			}
		}
	}

	system("pause");
	if (!ImageWriter_Scene_RemoveAllEntities(instance, &plainReply))
	{
		std::cerr << "ImageWriter error: " << plainReply.message << std::endl;
	}

	//system("pause");
	//AddEllipse(instance, statusMessage, 
	//	{ .positionX = cameraTransform.scaleX, .positionY = cameraTransform.scaleY, .scaleX = 1, .scaleY = 1, .angle = 0 },
	//	{.red = 1, .green = 0, .blue = 0, .alpha = 1});
	//
	//AddRectangle(instance, statusMessage, 
	//	{ .positionX = -cameraTransform.scaleX, .positionY = -cameraTransform.scaleY, .scaleX = 3, .scaleY = 2, .angle = 20 }, 
	//	{ .red = 0.5f, .green = 0.5f, .blue = 1, .alpha = 1 });

	//AddTriangle(instance, statusMessage,
	//	{ .positionX = 0, .positionY = 0, .scaleX = 1, .scaleY = 1, .angle = 60 },
	//	{ .red = 0, .green = 1, .blue = 0, .alpha = 1 },
	//	{ 1, -1 }, { 0,2 }, { -1,-1 });

	//printf("%s\n", statusMessage);

	//system("pause");
	//ClearImage(instance, statusMessage);

	//system("pause");
	//AddRectangle(instance, statusMessage, 
	//	{ .positionX = 0, .positionY = 0, .scaleX = cameraTransform.scaleX - 1, .scaleY = cameraTransform.scaleY - 1, .angle = -30 }, 
	//	{ .red = 0.5f, .green = 1, .blue = 0.5f, .alpha = 0.35f });

	//AddTriangle(instance, statusMessage,
	//	{ .positionX = 0, .positionY = 0, .scaleX = 1, .scaleY = 1, .angle = 0 },
	//	{ .red = 1, .green = 1, .blue = 0, .alpha = 1 },
	//	{ 3,1 }, { 1,3 }, { -2,0 });

	//AddEllipse(instance, statusMessage, 
	//	{ .positionX = 3, .positionY = -2, .scaleX = 3, .scaleY = 2, .angle = -10 }, 
	//	{ .red = 0, .green = 0, .blue = 1, .alpha = 0.4f });
	//

	//system("pause");
	//ClearImage(instance, statusMessage);

	//system("pause");
	//AddEllipse(instance, statusMessage, 
	//	{ .positionX = -5, .positionY = 3, .scaleX = 1, .scaleY = 4, .angle = -25 }, 
	//	{ .red = 0, .green = 1, .blue = 1, .alpha = 0.6f });

	//AddEllipse(instance, statusMessage, 
	//	{ .positionX = 7, .positionY = -4, .scaleX = 0.5f, .scaleY = 1.5f, .angle = 60 }, 
	//	{ .red = 1, .green = 1, .blue = 0, .alpha = 0.2f });

	//AddRectangle(instance, statusMessage, 
	//	{ .positionX = -8, .positionY = -1, .scaleX = 1, .scaleY = 1, .angle = 12 }, 
	//	{ .red = 1, .green = 0.5, .blue = 1, .alpha = 0.6f });
}
int main(void)
{
	HImageWriterInstance instance = {};

	if (!ImageWriter_Instance_Intialize(&instance)) {
		return EXIT_FAILURE;
	}

	// Create an isolated window with a handle value of 0
	int windowHwnd = 0;
	ImageWriter_Scene_Init(instance, windowHwnd, { .gridViewMode = GRID_VIEW_MODE_BACKGROUND });
	std::thread imageWriterThread(runImageWriterFlow, instance);


	auto prevTime = std::chrono::steady_clock::now();

	while (ImageWriter_Scene_IsSelfManagedRenderWindowOpen(instance)) {
		auto currentTime = std::chrono::steady_clock::now();

		std::chrono::duration<float> elapsed = currentTime - prevTime;

		float deltaTime = elapsed.count();

		std::this_thread::sleep_for(std::chrono::milliseconds(8));

		ImageWriter_Scene_UpdateFrame(instance, deltaTime);

		prevTime = currentTime;
	}

	ImageWriter_Scene_Dispose(instance);

	if (imageWriterThread.joinable()) {
		imageWriterThread.join();
	}
	ImageWriter_Instance_Dispose(&instance);
	
	return EXIT_SUCCESS;
}