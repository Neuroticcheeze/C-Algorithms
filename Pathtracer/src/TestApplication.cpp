#include "TestApplication.h"
#include "deps/gl_core_4_4.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <imgui.h>
#include "deps/imgui_impl_glfw_gl3.h"

#include "Camera.h"
#include "Gizmos.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "deps\tiny_obj_loader.h"

#include "..\ImGuizmo.hpp"

TestApplication::TestApplication()
	: m_camera(nullptr) {
}

TestApplication::~TestApplication() {

}

bool TestApplication::startup() {

	globalApp = this;

	// create a basic window
	createWindow(g_name.c_str(), g_width, g_height);

	// start the gizmo system that can draw basic shapes
	Gizmos::create();

	// create a camera
	m_camera = new Camera(glm::radians(70.0F), static_cast<float>(g_width) / g_height, 0.1f, 1000.f);
	m_camera->setLookAtFrom(glm::vec3(10), glm::vec3(0));

	// setup imgui binding
	ImGui_ImplGlfwGL3_Init(m_window, true);

	//////////////////////////////////////////////////////////////////////////
	// YOUR STARTUP CODE HERE
	//////////////////////////////////////////////////////////////////////////
	if (g_Init) g_Init();

	return true;
}

void TestApplication::windowResize(int w, int h)
{
	if (m_camera)
		m_camera->setPerspective(glm::radians(70.0F), static_cast<float>(w) / h, 0.1f, 1000.f);
}

void TestApplication::shutdown() {

	//////////////////////////////////////////////////////////////////////////
	// YOUR SHUTDOWN CODE HERE
	//////////////////////////////////////////////////////////////////////////
	if (g_Stop) g_Stop();

	// shutdown imgui
	ImGui_ImplGlfwGL3_Shutdown();

	// delete our camera and cleanup gizmos
	delete m_camera;
	Gizmos::destroy();

	// destroy our window properly
	destroyWindow();
}


void TestApplication::EditTransform(float* p_matrix, bool enableTranslation, bool enableRotation, bool enableScale)
{
	static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::ROTATE);
	static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);

	int enableCount = enableTranslation + enableRotation + enableScale;
	auto a = enableTranslation ? ImGuizmo::TRANSLATE : (enableRotation ? ImGuizmo::ROTATE : ImGuizmo::SCALE);
	mCurrentGizmoOperation = a;

	if (enableTranslation)
	{
		if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
			mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
		if (ImGui::IsKeyPressed(90))
			mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	}

	if (enableRotation)
	{
		ImGui::SameLine();
		if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
			mCurrentGizmoOperation = ImGuizmo::ROTATE;
		if (ImGui::IsKeyPressed(69))
			mCurrentGizmoOperation = ImGuizmo::ROTATE;
	}

	if (enableScale)
	{
		ImGui::SameLine();
		if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
			mCurrentGizmoOperation = ImGuizmo::SCALE;
		if (ImGui::IsKeyPressed(82)) // r Key
			mCurrentGizmoOperation = ImGuizmo::SCALE;
	}

	float matrixTranslation[3], matrixRotation[3], matrixScale[3];
	ImGuizmo::DecomposeMatrixToComponents(p_matrix, matrixTranslation, matrixRotation, matrixScale);
	ImGui::InputFloat3("Tr", matrixTranslation, 3);
	ImGui::InputFloat3("Rt", matrixRotation, 3);
	ImGui::InputFloat3("Sc", matrixScale, 3);
	ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, p_matrix);

	if (mCurrentGizmoOperation != ImGuizmo::SCALE)
	{
		if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
			mCurrentGizmoMode = ImGuizmo::LOCAL;
		ImGui::SameLine();
		if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
			mCurrentGizmoMode = ImGuizmo::WORLD;
	}
	static bool useSnap(false);
	if (ImGui::IsKeyPressed(83))
		useSnap = !useSnap;
	ImGui::Checkbox("", &useSnap);
	ImGui::SameLine();
	glm::vec3 snap;
	static glm::vec3 mSnapTranslation(0.0F);
	static glm::vec3 mSnapRotation(0.0F);
	static glm::vec3 mSnapScale(0.0F);
	switch (mCurrentGizmoOperation)
	{
	case ImGuizmo::TRANSLATE:
		ImGui::InputFloat3("Snap", &mSnapTranslation.x);
		snap = mSnapTranslation;
		break;
	case ImGuizmo::ROTATE:
		ImGui::InputFloat("Angle Snap", &mSnapRotation.x);
		snap = mSnapRotation;
		break;
	case ImGuizmo::SCALE:
		ImGui::InputFloat("Scale Snap", &mSnapScale.x);
		snap = mSnapScale;
		break;
	}
	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	ImGuizmo::Manipulate(glm::value_ptr(m_camera->getView()), glm::value_ptr(m_camera->getProjection()), mCurrentGizmoOperation, mCurrentGizmoMode, p_matrix, NULL, useSnap ? &snap.x : NULL);
}

bool TestApplication::update(float deltaTime)
{
	glfwPollEvents();
	// close the application if the window closes
	if (glfwWindowShouldClose(m_window) ||
		glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		return false;

	// start the imgui frame
	ImGui_ImplGlfwGL3_NewFrame();
	ImGuizmo::BeginFrame();

	// update the camera's movement
	m_camera->update(deltaTime);

	glm::vec4 positionVec = m_camera->getTransform()[3];
	glm::vec4 forwardVec = m_camera->getTransform()[2];
	glm::vec4 upVec = m_camera->getTransform()[1];
	glm::vec4 rightVec = m_camera->getTransform()[0];

	// clear the gizmos out for this frame
	Gizmos::clear();

	//////////////////////////////////////////////////////////////////////////
	// YOUR UPDATE CODE HERE
	//////////////////////////////////////////////////////////////////////////
	if (g_Update) g_Update(glm::value_ptr(positionVec), glm::value_ptr(forwardVec), glm::value_ptr(upVec), glm::value_ptr(rightVec), deltaTime);

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
	glm::mat4 guiMatrix = glm::ortho<float>(0, (float)width, 0, (float)height);

	Gizmos::draw2D(guiMatrix);

	ImGui::Render();

	Sleep(8);
}