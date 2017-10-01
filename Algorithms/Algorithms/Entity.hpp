#pragma once
#ifndef _H_ENTITY
#define _H_ENTITY

#include "Geometry.hpp"
#include <vector>

struct Node;

class Entity
{
public:

	Entity();
	Entity(const Entity &other);
	Entity(const Vector3D &startpos, const Vector3D &size = Vector3D(1.0F));

	void update(const float &dt, bool repel);

	AxisAlignedBoundingBox getBounds() const;

	static Vector3D MAX_ROAM_AREA;
	
	std::vector<Node*> nodes;

private:

	float life = 0.0F;
	Vector3D position, targetpos;
	AxisAlignedBoundingBox bounds;
};

#endif//_H_ENTITY