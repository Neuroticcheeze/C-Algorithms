#pragma once
#ifndef COLOUR_H_
#define COLOUR_H_

#include <glm\common.hpp>
using glm::vec2;
using glm::vec3;
using glm::vec4;

typedef vec4 _colour;

struct colour
{
	colour(const vec3 &other) : colour(other.x, other.y, other.z) {}
	colour(const vec4 &other) : colour(other.x, other.y, other.z, other.w) {}
	colour(float r = 0.0F, float g = 0.0F, float b = 0.0F, float l = 0.0F) : r(r), g(g), b(b), l(l) {}

	operator vec3() const
	{
		return vec3(r, g, b);
	}

	operator vec4() const
	{
		return vec4(r, g, b, l);
	}

	//Red
	float r;
	//Green
	float g;
	//Blue
	float b;
	//Luminosity
	float l;
};

#endif//COLOUR_H_