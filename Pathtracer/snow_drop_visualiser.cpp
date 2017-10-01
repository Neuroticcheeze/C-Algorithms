#include "snow_drop_visualiser.hpp"
#include "src\TestApplication.h"

TestApplication* g_app = nullptr;

sdvContext::sdvContext(const char *name, unsigned width, unsigned height)
{
	if (!g_app)
		g_app = new TestApplication();

	g_app->g_name = name;
	g_app->g_width = width;
	g_app->g_height = height;
}

sdvContext::~sdvContext()
{
	if (g_app)
	{
		g_app->g_Init = nullptr;
		g_app->g_Update = nullptr;
		g_app->g_Stop = nullptr;
		delete g_app;
		g_app = nullptr;
	}
}

void sdvContext::hookInit(sdv_fptr_NoArg callback)
{
	if (g_app)
	g_app->g_Init = callback;
}

void sdvContext::hookUpdate(sdv_fptr_CamVecs4DT callback)
{
	if (g_app)
	g_app->g_Update = callback;
}

void sdvContext::hookStop(sdv_fptr_NoArg callback)
{
	if (g_app)
	g_app->g_Stop = callback;
}

void sdvContext::start()
{
	if (g_app)
	if (g_app->startup())
		g_app->run();

	if (g_app)
	g_app->shutdown();
}

#pragma region Graphics

#include "src\Gizmos.h"
#include <glm\ext.hpp>

void sdvGraphics::addPoint(const pfloat3 a_pos, const pfloat4 a_colour)
{
	Gizmos::addPoint(glm::make_vec3(a_pos), glm::make_vec4(a_colour));
}

void sdvGraphics::addLine(const pfloat3 a_rv0, const pfloat3 a_rv1, const pfloat4 a_colour)
{
	Gizmos::addLine(glm::make_vec3(a_rv0), glm::make_vec3(a_rv1), glm::make_vec4(a_colour));
}

// Adds a single debug line
void sdvGraphics::addLine(const pfloat3 a_rv0, const pfloat3 a_rv1,
	const pfloat4 a_colour0, const pfloat4 a_colour1)
{
	Gizmos::addLine(glm::make_vec3(a_rv0), glm::make_vec3(a_rv1), glm::make_vec4(a_colour0), glm::make_vec4(a_colour1));
}

// Adds a triangle.
void		sdvGraphics::addTri(const pfloat3 a_rv0, const pfloat3 a_rv1, const pfloat3 a_rv2, const pfloat4 a_colour)
{
	Gizmos::addTri(glm::make_vec3(a_rv0), glm::make_vec3(a_rv1), glm::make_vec3(a_rv2), glm::make_vec4(a_colour));
}

// Adds 3 unit-length lines (red,green,blue) representing the 3 axis of a transform, 
// at the transform's translation. Optional scale available.
void		sdvGraphics::addTransform(const pfloat4x4 a_transform, float a_fScale)
{
	Gizmos::addTransform(glm::make_mat4(a_transform), a_fScale);
}

// Adds a wireframe Axis-Aligned Bounding-Box with optional transform for rotation/translation.
void		sdvGraphics::addAABB(const pfloat3 a_center, const pfloat3 a_extents,
	const pfloat4 a_colour, const pfloat4x4 a_transform)
{
	Gizmos::addAABB(glm::make_vec3(a_center), glm::make_vec3(a_extents), glm::make_vec4(a_colour), a_transform ? &glm::make_mat4(a_transform) : nullptr);
}

// Adds an Axis-Aligned Bounding-Box with optional transform for rotation.
void		sdvGraphics::addAABBFilled(const pfloat3 a_center, const pfloat3 a_extents,
	const pfloat4 a_fillColour, const pfloat4x4 a_transform)
{
	Gizmos::addAABBFilled(glm::make_vec3(a_center), glm::make_vec3(a_extents), glm::make_vec4(a_fillColour), a_transform ? &glm::make_mat4(a_transform) : nullptr);
}

// Adds a cylinder aligned to the Y-axis with optional transform for rotation.
void		sdvGraphics::addCylinderFilled(const pfloat3 a_center, float a_radius, float a_fHalfLength,
	unsigned int a_segments, const pfloat4 a_fillColour, const pfloat4x4 a_transform)
{
	Gizmos::addCylinderFilled(glm::make_vec3(a_center), a_radius, a_fHalfLength, a_segments, glm::make_vec4(a_fillColour), a_transform ? &glm::make_mat4(a_transform) : nullptr);
}

// Adds a double-sided hollow ring in the XZ axis with optional transform for rotation.
// If a_rvFilLColour.w == 0 then only an outer and inner line is drawn.
void		sdvGraphics::addRing(const pfloat3 a_center, float a_innerRadius, float a_outerRadius,
	unsigned int a_segments, const pfloat4 a_fillColour, const pfloat4x4 a_transform)
{
	Gizmos::addRing(glm::make_vec3(a_center), a_innerRadius, a_outerRadius, a_segments, glm::make_vec4(a_fillColour), a_transform ? &glm::make_mat4(a_transform) : nullptr);
}

// Adds a double-sided disk in the XZ axis with optional transform for rotation.
// If a_rvFilLColour.w == 0 then only an outer line is drawn.
void		sdvGraphics::addDisk(const pfloat3 a_center, float a_radius,
	unsigned int a_segments, const pfloat4 a_fillColour, const pfloat4x4 a_transform)
{
	Gizmos::addDisk(glm::make_vec3(a_center), a_radius, a_segments, glm::make_vec4(a_fillColour), a_transform ? &glm::make_mat4(a_transform) : nullptr);
}

// Adds an arc, around the Y-axis
// If a_rvFilLColour.w == 0 then only an outer line is drawn.
void		sdvGraphics::addArc(const pfloat3 a_center, float a_rotation,
	float a_radius, float a_halfAngle,
	unsigned int a_segments, const pfloat4 a_fillColour, const pfloat4x4 a_transform)
{
	Gizmos::addArc(glm::make_vec3(a_center), a_rotation, a_radius, a_halfAngle, a_segments, glm::make_vec4(a_fillColour), a_transform ? &glm::make_mat4(a_transform) : nullptr);
}

// Adds an arc, around the Y-axis, starting at the inner radius and extending to the outer radius
// If a_rvFilLColour.w == 0 then only an outer line is drawn.
void		sdvGraphics::addArcRing(const pfloat3 a_center, float a_rotation,
	float a_innerRadius, float a_outerRadius, float a_arcHalfAngle,
	unsigned int a_segments, const pfloat4 a_fillColour, const pfloat4x4 a_transform)
{
	Gizmos::addArcRing(glm::make_vec3(a_center), a_rotation, a_innerRadius, a_outerRadius, a_arcHalfAngle, a_segments, glm::make_vec4(a_fillColour), a_transform ? &glm::make_mat4(a_transform) : nullptr);
}

// Adds a Sphere at a given position, with a given number of rows, and columns, radius and a max and min long and latitude
void		sdvGraphics::addSphere(const pfloat3 a_center, float a_radius, int a_rows, int a_columns, const pfloat4 a_fillColour,
	const pfloat4x4 a_transform, float a_longMin, float a_longMax,
	float a_latMin, float a_latMax)
{
	Gizmos::addSphere(glm::make_vec3(a_center), a_radius, a_rows, a_columns, glm::make_vec4(a_fillColour), a_transform ? &glm::make_mat4(a_transform) : nullptr, a_longMin, a_longMax, a_latMin, a_latMax);
}

// Adds a single Hermite spline curve
void		sdvGraphics::addHermiteSpline(const pfloat3 a_start, const pfloat3 a_end,
	const pfloat3 a_tangentStart, const pfloat3 a_tangentEnd, unsigned int a_segments, const pfloat4 a_colour)
{
	Gizmos::addHermiteSpline(glm::make_vec3(a_start), glm::make_vec3(a_end), glm::make_vec3(a_tangentStart), glm::make_vec3(a_tangentEnd), a_segments, glm::make_vec4(a_colour));
}

// Adds a Frustum at a given position, rotation, fov, aspect ratio, near and far planes.
void		sdvGraphics::addFrustum(const pfloat3 a_position, const pfloat3 a_lookVector, float a_fov, float a_aspect, float a_near, float a_far, const pfloat4 a_colour, const pfloat4x4 a_transform)
{
	Gizmos::addFrustum(glm::make_vec3(a_position), glm::make_vec3(a_lookVector), a_fov, a_aspect, a_near, a_far, glm::make_vec4(a_colour), a_transform ? &glm::make_mat4(a_transform) : nullptr);
}

// 2-dimensional gizmos
void		sdvGraphics::add2DLine(const pfloat2 a_start, const pfloat2 a_end, const pfloat4 a_colour)
{
	Gizmos::add2DLine(glm::make_vec2(a_start), glm::make_vec2(a_end), glm::make_vec4(a_colour));
}
void		sdvGraphics::add2DLine(const pfloat2 a_start, const pfloat2 a_end, const pfloat4 a_colour0, const pfloat4 a_colour1)
{
	Gizmos::add2DLine(glm::make_vec2(a_start), glm::make_vec2(a_end), glm::make_vec4(a_colour0), glm::make_vec4(a_colour1));
}
void		sdvGraphics::add2DTri(const pfloat2 a_0, const pfloat2 a_1, const pfloat2 a_2, const pfloat4 a_colour)
{
	Gizmos::add2DTri(glm::make_vec2(a_0), glm::make_vec2(a_1), glm::make_vec2(a_2), glm::make_vec4(a_colour));
}
void		sdvGraphics::add2DAABB(const pfloat2 a_center, const pfloat2 a_extents, const pfloat4 a_colour, const pfloat4x4 a_transform)
{
	Gizmos::add2DAABB(glm::make_vec2(a_center), glm::make_vec2(a_extents), glm::make_vec4(a_colour), a_transform ? &glm::make_mat4(a_transform) : nullptr);
}
void		sdvGraphics::add2DAABBFilled(const pfloat2 a_center, const pfloat2 a_extents, const pfloat4 a_colour, const pfloat4x4 a_transform)
{
	Gizmos::add2DAABBFilled(glm::make_vec2(a_center), glm::make_vec2(a_extents), glm::make_vec4(a_colour), a_transform ? &glm::make_mat4(a_transform) : nullptr);
}
void		sdvGraphics::add2DCircle(const pfloat2 a_center, float a_radius, unsigned int a_segments, const pfloat4 a_colour, const pfloat4x4 a_transform)
{
	Gizmos::add2DCircle(glm::make_vec2(a_center), a_radius, a_segments, glm::make_vec4(a_colour), a_transform ? &glm::make_mat4(a_transform) : nullptr);
}

#pragma endregion

#pragma region Gizmos

#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>

void sdvGizmos::addPositionHandle(pfloat3 p_pos)
{
	if (g_app)
	{
		glm::mat4 transformation = glm::translate(glm::make_vec3(p_pos));
		g_app->EditTransform(glm::value_ptr(transformation), true, false, false);

		glm::vec3 scale;
		glm::quat rotation;
		glm::vec3 translation;
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(transformation, scale, rotation, translation, skew, perspective);

		p_pos[0] = translation.x;
		p_pos[1] = translation.y;
		p_pos[2] = translation.z;
	}
}

void sdvGizmos::addRotateHandle(pfloat3 p_rot)
{
	if (g_app)
	{
		glm::mat4 transformation = glm::toMat4(glm::quat(glm::make_vec3(p_rot)));
		g_app->EditTransform(glm::value_ptr(transformation), false, true, false);

		glm::vec3 scale;
		glm::quat rotation;
		glm::vec3 translation;
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(transformation, scale, rotation, translation, skew, perspective);

		glm::vec3 rotationEuler = glm::eulerAngles(rotation);
		p_rot[0] = rotationEuler.x;
		p_rot[1] = rotationEuler.y;
		p_rot[2] = rotationEuler.z;
	}
}

void sdvGizmos::addScaleHandle(pfloat3 p_scal)
{
	if (g_app)
	{
		glm::mat4 transformation = glm::scale(glm::make_vec3(p_scal));
		g_app->EditTransform(glm::value_ptr(transformation), false, false, true);

		glm::vec3 scale;
		glm::quat rotation;
		glm::vec3 translation;
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(transformation, scale, rotation, translation, skew, perspective);

		p_scal[0] = scale.x;
		p_scal[1] = scale.y;
		p_scal[2] = scale.z;
	}
}

void sdvGizmos::addTransformHandle(pfloat4x4 p_mat)
{
	g_app->EditTransform(p_mat);
}
#pragma endregion