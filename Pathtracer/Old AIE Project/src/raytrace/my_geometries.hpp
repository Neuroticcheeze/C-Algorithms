#pragma once

#include "raytrace.hpp"

struct sphere : public geometry
{
	sphere(const vec3 &pos = vec3(), const float &radius = 1.0F);

	bool test(const ray &ray_, hit_info &result) const;

	vec3 pos;
	float radius;
};

struct triangle_vertex
{
	vec3 pos, tan, norm;
	vec2 tc;
};

struct triangle : public geometry
{
	triangle(const triangle_vertex &vert0 = triangle_vertex(), const triangle_vertex &vert1 = triangle_vertex(), const triangle_vertex &vert2 = triangle_vertex());

	bool test(const ray &ray_, hit_info &result) const;

	triangle_vertex vert0, vert1, vert2;
};

struct plane : public geometry
{
	plane(const vec3 &pos = vec3(), const vec3 &normal = vec3(), const vec3 &tangent = vec3());

	bool test(const ray &ray_, hit_info &result) const;

	vec3 pos, normal, tangent;
};