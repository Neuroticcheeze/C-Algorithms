#pragma once

#include "list.hpp"
#include <glm\vec3.hpp>
using glm::vec3;

typedef unsigned char byte;

extern struct region;
extern class oct_tree;
extern struct oct_tree_node;

struct bounding_volume
{
	virtual inline void contact(const region &reg, bool &result) = 0;
	virtual void draw() = 0;
};

struct geometry2
{
protected:
	geometry2(bounding_volume *boundingVolume);

public:
	~geometry2();
	void draw() { m_boundingVolume->draw(); }
	bounding_volume *m_boundingVolume;
	ptr_list<oct_tree_node*> m_containingNodes;
};

struct region
{
	region();
	region(const vec3 &extent0, const vec3 &extent1);

	vec3 
		m_min, m_max, 
		m_center, m_halfExtents;
};

struct oct_tree_node
{
	oct_tree_node();
	~oct_tree_node();

	oct_tree * m_container = NULL;
	oct_tree_node * m_parent = NULL;
	oct_tree_node * m_children[8] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
	byte m_depthLevel = 0;
	region m_reg;
	int m_index = 0, m_masterIndex = 0, m_subDepthCount = 0;
	ptr_list<geometry2*> m_occupants;
};

class oct_tree
{
public:
	oct_tree(byte maxDepth, const region &region);
	~oct_tree();

	bool insertGeometry(geometry2 *geo_obj);
	bool removeGeometry(geometry2 *geo_obj);

	//TODO: add query functions
	void draw();
private:

	friend extern struct oct_tree_node;

	ptr_list<geometry2*> m_geoList;
	byte m_maxDepth;
	oct_tree_node *m_root;
	oct_tree_node **m_nodeMaster;
	unsigned m_nodeMasterSparseSize, m_nodeMasterCapacity;

	oct_tree_node **m_traversalChildren;
	byte *m_traversalChildrenCounts;

	friend extern void __Subdiv(oct_tree *val0, oct_tree_node *val0Node);
	friend extern void _Combine(oct_tree *val0, oct_tree_node *val0Node);
	friend extern inline int _MasterIndex_(byte depth, int parentIndex, int childIndex);
	friend extern void __GeomForgetNodes(oct_tree_node *val0);
	friend extern void __NodeForgetGeoms(geometry2 *val0);
	friend extern bool __Insert(oct_tree *val0, oct_tree_node *val0Node, geometry2 *val1, oct_tree_node **val2, byte &val2Count);
};