#include <NaezanCraft.h>
#include "Global.h"

int main(int argc, char** argv)
{
	app = new Application();
	app->Run();
	delete app;

	return 0;
}