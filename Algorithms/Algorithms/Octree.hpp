#pragma once
#ifndef _H_OCTREE
#define _H_OCTREE

#include "Geometry.hpp"
#include "Entity.hpp"
#include <vector>

struct Node
{
	Node();
	Node(const Node& other);
	~Node() noexcept;
	Node& operator= (const Node& other);
	
	Node **children = nullptr;
	std::vector<Entity*> population;
	AxisAlignedBoundingBox bounds;
};

typedef void(*fn_perNode)(const Node *node);

//A static octree implementation (has to be rebuilt every time an element is added or removed); ideal for pathtracing
class Octree
{
public:
	Octree();

	//Rule of Three
	Octree(const Octree& other);
	~Octree() noexcept;
	Octree& operator= (const Octree& other);

	static Octree *Build(AxisAlignedBoundingBox volume, int maxSubdivs, int maxClutter, Entity *entities, int entitiesCount);
	static void ForEach(const Octree *octree, fn_perNode action);

private:
	Node *root;
};

#endif//_H_OCTREE