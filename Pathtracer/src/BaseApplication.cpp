#include "BaseApplication.h"
#include "deps/gl_core_4_4.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>

BaseApplication *globalApp;

bool BaseApplication::createWindow(const char* title, int width, int height) {

	if (glfwInit() == GL_FALSE)
		return false;

	m_window = glfwCreateWindow(width, height, title, nullptr, nullptr);
	if (m_window == nullptr) {
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(m_window);

	if (ogl_LoadFunctions() == ogl_LOAD_FAILED) {
		glfwDestroyWindow(m_window);
		glfwTerminate();
		return false;
	}

	glfwSetWindowSizeCallback(m_window, [](GLFWwindow*, int w, int h) { glViewport(0, 0, w, h); globalApp->windowResize(w, h); });

	auto major = ogl_GetMajorVersion();
	auto minor = ogl_GetMinorVersion();
	std::cout << "GL: " << major << "." << minor << std::endl;

	glClearColor(0.265F, 0.35F, 0.5F, 1);

	glEnable(GL_DEPTH_TEST);
	
	return true;
}

void BaseApplication::destroyWindow() {

	glfwDestroyWindow(m_window);
	glfwTerminate();
}

void BaseApplication::run() {

	double prevTime = glfwGetTime();
	double currTime = 0;

	while (currTime = glfwGetTime(),
		update((float)(currTime - prevTime))) {

		draw();
		glfwSwapBuffers(m_window);

		prevTime = currTime;
	}
}