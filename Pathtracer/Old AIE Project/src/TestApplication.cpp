#include "TestApplication.h"
#include "gl_core_4_4.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "Camera.h"
#include "Gizmos.h"

#include "raytrace/screen.hpp"
#include "raytrace/raytrace.hpp"
#include "raytrace/my_geometries.hpp"

using glm::vec3;
using glm::vec4;
using glm::mat4;

TestApplication::TestApplication()
	: m_camera(nullptr) {

}

TestApplication::~TestApplication() {

}

bool TestApplication::startup() {

	// create a basic window
	createWindow("AIE OpenGL Application", 1280, 720);

	// start the gizmo system that can draw basic shapes
	Gizmos::create();

	// create a camera
	m_camera = new Camera(glm::radians(70.0F), 16 / 9.f, 0.1f, 1000.f);
	m_camera->setLookAtFrom(vec3(40, 40, 80), vec3(0));
	
	//////////////////////////////////////////////////////////////////////////
	// YOUR STARTUP CODE HERE
	//////////////////////////////////////////////////////////////////////////
	
	return true;
}

void TestApplication::shutdown() {

	//////////////////////////////////////////////////////////////////////////
	// YOUR SHUTDOWN CODE HERE
	//////////////////////////////////////////////////////////////////////////

	// delete our camera and cleanup gizmos
	delete m_camera;
	Gizmos::destroy();

	// destroy our window properly
	destroyWindow();


	finaliseRaytracer();
	finaliseScreen();
}

/*
bool TestApplication::update(float deltaTime)
{
	static ray_tree **rt = nullptr;
	static camera camera_;

	if (!rt)
	{
		rt = new ray_tree*[10 * 10];
		srand(time(NULL));

		int sampleCount = 6;
		int bounceCount = 2;
		int w = 100,
			h = 100;

		//Add geomes to scene
		{
			addGeometry((new sphere(vec3(2, 0, -5), 2))->setMaterial(
				material(
					material::metal,
					colour(0.0F, 0.0F, 0.0F, 0.0F)
				)));
			addGeometry((new sphere(vec3(1.5F, 3, -5), 1.5F))->setMaterial(
				material(
					material::metal,
					colour(0.0F, 0.1F, 0.3F, 0.0F)
				)));
			addGeometry((new sphere(vec3(-4, -8, -3), 6))->setMaterial(
				material(
					material::glowy,
					colour(0.2F, 0.9F, 0.5F, 5.0F)
				)));
			addGeometry((new sphere(vec3(-1, 3, -6), 1.0F))->setMaterial(
				material(
					material::metal,
					colour(0.2F, 0.2F, 0.2F, 0.0F)
				)));
			addGeometry((new plane(vec3(-2, -2, -4), vec3(0, 1, 0)))->setMaterial(
				material(
					material::metal,
					colour(0.1F, 0.05F, 0.1F, 1.0F)
				)));
		}

		initialiseScreen(w, h);
		initialiseRaytracer(bounceCount, sampleCount);

		camera_ = camera(vec3(-0.5F, 0.0F, 9.0F), vec3(0.0F, 0.0F, -1.0F), glm::radians(70.0F), 0.1F, 100.0F);
		int s = 0;
		for (int x = 0; x < 100; x += 10)for (int y = 0; y < 100; y += 10)
		{
			ray camRay = camera_.project(x, y);
			rt[s] = new ray_tree;
			buildRayTree(camRay, rt[s]);
			++s;
		}
	}

	// close the application if the window closes
	if (glfwWindowShouldClose(m_window) ||
		glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		return false;

	// update the camera's movement
	m_camera->update(deltaTime);

	vec4 positionVec = m_camera->getTransform()[3];
	vec4 forwardVec = m_camera->getTransform()[2];
	vec4 rightVec = m_camera->getTransform()[0];

	

	// clear the gizmos out for this frame
	Gizmos::clear();

	//////////////////////////////////////////////////////////////////////////
	// YOUR UPDATE CODE HERE
	//////////////////////////////////////////////////////////////////////////

	drawSceneGeometryAndCamera(camera_);
	for (int i = 0; i < 100; ++i)
		drawRayTree(rt[i]);

	// return true, else the application closes
	return true;
}
*/

#include "octree\octree.hpp"
#include "octree\my_geoms_bvols.hpp"

bool TestApplication::update(float deltaTime)
{
	static oct_tree *ot = nullptr;
	static camera camera_;

	if (!ot)
	{
		ot = new oct_tree(5, region(vec3(-40), vec3(40)));
		srand(time(NULL));
		for (int t = 0; t < 50; ++t)
		{
			ot->insertGeometry(new geometry_sphere(glm::ballRand(40.0F), glm::linearRand(0.5F, 10.0F)));
		}
	}

	// close the application if the window closes
	if (glfwWindowShouldClose(m_window) ||
		glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		return false;

	// update the camera's movement
	m_camera->update(deltaTime);

	vec4 positionVec = m_camera->getTransform()[3];
	vec4 forwardVec = m_camera->getTransform()[2];
	vec4 rightVec = m_camera->getTransform()[0];



	// clear the gizmos out for this frame
	Gizmos::clear();

	//////////////////////////////////////////////////////////////////////////
	// YOUR UPDATE CODE HERE
	//////////////////////////////////////////////////////////////////////////

	ot->draw();

	// return true, else the application closes
	return true;
}

void TestApplication::draw() {

	// clear the screen for this frame
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//////////////////////////////////////////////////////////////////////////
	// DRAW YOUR THINGS HERE
	//////////////////////////////////////////////////////////////////////////

	// display the 3D gizmos
	Gizmos::draw(m_camera->getProjectionView());

	// get a orthographic projection matrix and draw 2D gizmos
	int width = 0, height = 0;
	glfwGetWindowSize(m_window, &width, &height);
	mat4 guiMatrix = glm::ortho<float>(0, (float)width, 0, (float)height);

	Gizmos::draw2D(guiMatrix);

	Sleep(8);
}