

#include "TestApplication.h"

#include <GLFW/glfw3.h>

int main()
{
	TestApplication* app = new TestApplication();
	if (app->startup())
		app->run();
	app->shutdown();

	return 0;
}