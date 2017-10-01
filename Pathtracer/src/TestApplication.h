#pragma once

#include "BaseApplication.h"

#include "..\snow_drop_visualiser.hpp"

// only needed for the camera picking
#include <glm/vec3.hpp>
#include <string>
class Camera;

class TestApplication : public BaseApplication {
public:

	TestApplication();
	virtual ~TestApplication();

	virtual bool startup();
	virtual void shutdown();

	virtual bool update(float deltaTime);
	virtual void draw();
	virtual void windowResize(int w, int h);

	int g_width = 640;
	int g_height = 480;
	std::string g_name = "Snow Drop Visualiser Application";
	sdv_fptr_NoArg g_Init = nullptr;
	sdv_fptr_CamVecs4DT g_Update = nullptr;
	sdv_fptr_NoArg g_Stop = nullptr;

	void EditTransform(float* p_matrix, bool enableTranslation = true, bool enableRotation = true, bool enableScale = true);

private:

	Camera*		m_camera;
};