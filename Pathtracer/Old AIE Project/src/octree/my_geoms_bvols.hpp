#pragma once
#include "octree.hpp"

struct bounding_volume_aabb : public bounding_volume
{
	bounding_volume_aabb(const vec3 &extent0, const vec3 &extent1);

	inline void contact(const region &reg, bool &result);
	void draw();
	region m_reg;
};

struct bounding_volume_sphere : public bounding_volume
{
	bounding_volume_sphere(const vec3 &center, const float &radius);

	inline void contact(const region &reg, bool &result);
	void draw();
	vec3 m_center;
	float m_radius;
};

struct geometry_sphere : public geometry2
{
	geometry_sphere(const vec3 &center, const float &radius);
	vec3 m_center;
	float m_radius;
};