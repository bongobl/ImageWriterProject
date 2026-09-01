#include <API.h>
#include <cstdlib>

int main(void)
{
	HImageWriterInstance imageWriterInstance = {};
	CreateImageWriterInstance(&imageWriterInstance);

	SetupImage(imageWriterInstance, 1920, 1080);
	ExportImage(imageWriterInstance);
	DestroyImageWriterInstance(&imageWriterInstance);
	return EXIT_SUCCESS;
}