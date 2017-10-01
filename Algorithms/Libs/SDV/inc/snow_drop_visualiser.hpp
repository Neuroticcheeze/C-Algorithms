#pragma once
#ifndef _H_SDV
#define _H_SDV

#ifndef SDV_API
#ifdef SDV_EXPORT
#define SDV_API __declspec(dllexport)   
#else  
#define SDV_API __declspec(dllimport)   
#endif
#endif

typedef float *pfloat2;
typedef float *pfloat3;
typedef float *pfloat4;
typedef float *pfloat4x4;

typedef void(*sdv_fptr_NoArg)();
typedef void(*sdv_fptr_CamVecs4DT)(const float *positionVec, const float *forwardVec, const float *upVec, const float *rightVec, const float &deltaTime);

SDV_API class sdvContext
{
public:
	sdvContext(const char *name, unsigned width, unsigned height);
	~sdvContext();
	void hookInit(sdv_fptr_NoArg callback);
	void hookUpdate(sdv_fptr_CamVecs4DT callback);
	void hookStop(sdv_fptr_NoArg callback);
	void start();
};

namespace sdvGraphics
{
	// Adds a single debug point. Transparency is not enabled for points. Set point size with glPointSize(...)
	SDV_API void addPoint(const pfloat3 a_pos, const pfloat4 a_colour);

	// Adds a single debug line
	SDV_API void addLine(const pfloat3 a_rv0, const pfloat3 a_rv1,
		const pfloat4 a_colour);

	// Adds a single debug line
	SDV_API void addLine(const pfloat3 a_rv0, const pfloat3 a_rv1,
		const pfloat4 a_colour0, const pfloat4 a_colour1);

	// Adds a triangle.
	SDV_API void addTri(const pfloat3 a_rv0, const pfloat3 a_rv1, const pfloat3 a_rv2, const pfloat4 a_colour);

	// Adds 3 unit-length lines (red,green,blue) representing the 3 axis of a transform, 
	// at the transform's translation. Optional scale available.
	SDV_API void addTransform(const pfloat4x4 a_transform, float a_fScale = 1.0f);

	// Adds a wireframe Axis-Aligned Bounding-Box with optional transform for rotation/translation.
	SDV_API void addAABB(const pfloat3 a_center, const pfloat3 a_extents,
		const pfloat4 a_colour, const pfloat4x4 a_transform = nullptr);

	// Adds an Axis-Aligned Bounding-Box with optional transform for rotation.
	SDV_API void addAABBFilled(const pfloat3 a_center, const pfloat3 a_extents,
		const pfloat4 a_fillColour, const pfloat4x4 a_transform = nullptr);

	// Adds a cylinder aligned to the Y-axis with optional transform for rotation.
	SDV_API void addCylinderFilled(const pfloat3 a_center, float a_radius, float a_fHalfLength,
		unsigned int a_segments, const pfloat4 a_fillColour, const pfloat4x4 a_transform = nullptr);

	// Adds a double-sided hollow ring in the XZ axis with optional transform for rotation.
	// If a_rvFilLColour.w == 0 then only an outer and inner line is drawn.
	SDV_API void addRing(const pfloat3 a_center, float a_innerRadius, float a_outerRadius,
		unsigned int a_segments, const pfloat4 a_fillColour, const pfloat4x4 a_transform = nullptr);

	// Adds a double-sided disk in the XZ axis with optional transform for rotation.
	// If a_rvFilLColour.w == 0 then only an outer line is drawn.
	SDV_API void addDisk(const pfloat3 a_center, float a_radius,
		unsigned int a_segments, const pfloat4 a_fillColour, const pfloat4x4 a_transform = nullptr);

	// Adds an arc, around the Y-axis
	// If a_rvFilLColour.w == 0 then only an outer line is drawn.
	SDV_API void addArc(const pfloat3 a_center, float a_rotation,
		float a_radius, float a_halfAngle,
		unsigned int a_segments, const pfloat4 a_fillColour, const pfloat4x4 a_transform = nullptr);

	// Adds an arc, around the Y-axis, starting at the inner radius and extending to the outer radius
	// If a_rvFilLColour.w == 0 then only an outer line is drawn.
	SDV_API void addArcRing(const pfloat3 a_center, float a_rotation,
		float a_innerRadius, float a_outerRadius, float a_arcHalfAngle,
		unsigned int a_segments, const pfloat4 a_fillColour, const pfloat4x4 a_transform = nullptr);

	// Adds a Sphere at a given position, with a given number of rows, and columns, radius and a max and min long and latitude
	SDV_API void addSphere(const pfloat3 a_center, float a_radius, int a_rows, int a_columns, const pfloat4 a_fillColour,
		const pfloat4x4 a_transform = nullptr, float a_longMin = 0.f, float a_longMax = 360,
		float a_latMin = -90, float a_latMax = 90);

	// Adds a single Hermite spline curve
	SDV_API void addHermiteSpline(const pfloat3 a_start, const pfloat3 a_end,
		const pfloat3 a_tangentStart, const pfloat3 a_tangentEnd, unsigned int a_segments, const pfloat4 a_colour);

	// Adds a Frustum at a given position, rotation, fov, aspect ratio, near and far planes.
	SDV_API void addFrustum(const pfloat3 a_position, const pfloat3 a_lookVector, float a_fov, float a_aspect, float a_near, float a_far, const pfloat4 a_colour, const pfloat4x4 a_transform = nullptr);

	// 2-dimensional gizmos
	SDV_API void add2DLine(const pfloat2 a_start, const pfloat2 a_end, const pfloat4 a_colour);
	SDV_API void add2DLine(const pfloat2 a_start, const pfloat2 a_end, const pfloat4 a_colour0, const pfloat4 a_colour1);
	SDV_API void add2DTri(const pfloat2 a_0, const pfloat2 a_1, const pfloat2 a_2, const pfloat4 a_colour);
	SDV_API void add2DAABB(const pfloat2 a_center, const pfloat2 a_extents, const pfloat4 a_colour, const pfloat4x4 a_transform = nullptr);
	SDV_API void add2DAABBFilled(const pfloat2 a_center, const pfloat2 a_extents, const pfloat4 a_colour, const pfloat4x4 a_transform = nullptr);
	SDV_API void add2DCircle(const pfloat2 a_center, float a_radius, unsigned int a_segments, const pfloat4 a_colour, const pfloat4x4 a_transform = nullptr);
}

namespace sdvGizmos 
{
	SDV_API void addPositionHandle(pfloat3 p_pos);
	SDV_API void addRotateHandle(pfloat3 p_rot);
	SDV_API void addScaleHandle(pfloat3 p_scal);
	SDV_API void addTransformHandle(pfloat4x4 p_mat);
}

namespace sdvGui
{
}

#endif//_H_SDV