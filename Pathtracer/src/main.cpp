#include "..\snow_drop_visualiser.hpp"
#include <imgui.h>
#include "..\ImGuizmo.hpp"



int main()
{
	sdvContext *ctx = new sdvContext("Octree", 1200, 960);


	ctx->hookUpdate([](const float *positionVec, const float *forwardVec, const float *upVec, const float *rightVec, const float &deltaTime) {
		static float center[] = { 0.0F, 0.0F, 0.0F };
		float colour[] = { 1.0F, 0.0F, 0.0F, 1.0F };

		sdvGizmos::addRotateHandle(center);
		sdvGraphics::addSphere(center, 2, 32, 16, colour);
	});

	ctx->start();
	delete ctx;

	return 0;
}