#include "octree.hpp"
#include <stdexcept>
#include <glm\glm.hpp>
#include "..\Gizmos.h"

using glm::pow;
using glm::min;
using glm::max;
using std::find;
using std::invalid_argument;

#define __OCTREE_NODE_TOTAL(D, T) ((pow<T, T>(8, D + 1) - 1) / 7)
#define __Safe_Subdiv(tree, node) if (node->m_subDepthCount == 0 && tree->m_maxDepth > 0) __Subdiv(tree, node);
#define __Introduce(geom, node) node->m_occupants.add(geom); geom->m_containingNodes.add(node);


geometry2::geometry2(bounding_volume *boundingVolume)
{
	m_boundingVolume = boundingVolume;
}

geometry2::~geometry2()
{
	delete m_boundingVolume;
}

region::region()
{

}

region::region(const vec3 &extent0, const vec3 &extent1) :
	m_min(min(extent0, extent1)),
	m_max(max(extent0, extent1)),
	m_center((m_min + m_max) * 0.5F),
	m_halfExtents((m_max - m_min) * 0.5F)
{
}

oct_tree_node::oct_tree_node()
{

}

oct_tree_node::~oct_tree_node()
{
}

oct_tree::oct_tree(byte maxDepth, const region &region) :
	m_maxDepth(maxDepth)
{
	// set the node master up
	m_nodeMasterCapacity = __OCTREE_NODE_TOTAL(static_cast<int>(m_maxDepth), int);
	m_nodeMaster = new oct_tree_node*[m_nodeMasterCapacity];
	memset(m_nodeMaster, 0, sizeof(void*) * m_nodeMasterCapacity);
	m_nodeMasterSparseSize = 1;
	
	// build traversal children array
	m_traversalChildren = new oct_tree_node*[m_nodeMasterCapacity];// as big as the master list, the first index must be the root node
	m_traversalChildrenCounts = new byte[m_nodeMasterCapacity];
	m_traversalChildren[0] = m_root;
	m_traversalChildrenCounts[0] = 1;

	// initialise the root node
	m_root = new oct_tree_node;
	m_root->m_container = this;
	m_root->m_depthLevel = 0;
	m_root->m_reg = region;
	m_root->m_index = 0;
	m_root->m_masterIndex = 0;
	m_root->m_subDepthCount = 0;

	// add root to octree master list
	m_nodeMaster[0] = m_root;
}

oct_tree::~oct_tree()
{
	// delete all nodes and free the master array
	static oct_tree_node *elem_;
	if (m_nodeMaster)
	{
		for (int elem = 0; elem < m_nodeMasterCapacity; ++elem)
		{
			elem_ = *(m_nodeMaster + elem);

			if (elem_)
			{
				__GeomForgetNodes(elem_);
				delete elem_;
				elem_ = nullptr;
			}
		}

		delete[] m_nodeMaster;
	}

	// free the traversal children array
	if (m_traversalChildren)
	{
		delete[] m_traversalChildren;
	}

	if (m_traversalChildrenCounts)
	{
		delete[] m_traversalChildrenCounts;
	}
}

bool oct_tree::insertGeometry(geometry2 *geo_obj)
{
	assert(m_root);

	if (!geo_obj)
		throw invalid_argument("'geo_obj' must be a valid pointer");

	// static variables used below
	oct_tree_node *elem_;
	static bool initContactResult;
	int index;
		
	geo_obj->m_boundingVolume->contact(m_root->m_reg, initContactResult);

	// if geom doesn't even pass the first node, then don't do anything and return
	if (!initContactResult)
		return false;

	// initialise children counts every call except the root node
	memset(&m_traversalChildrenCounts[1], 0, (m_nodeMasterCapacity - 1) * sizeof(byte));
	m_traversalChildren[0] = m_root;
	m_traversalChildrenCounts[0] = 1;
	//__Insert(this, m_root, geo_obj, &m_traversalChildren[1], m_traversalChildrenCounts[1]);


	// per depth level
	for (int depthLvl = 0; depthLvl < m_maxDepth; ++depthLvl)
	{
		int childCount = m_traversalChildrenCounts[depthLvl];

		// per intersected child in the depth level
		for (int child = 0; child < childCount; ++child)
		{
			int srcNode = __OCTREE_NODE_TOTAL(static_cast<int>(depthLvl - 1), int);
			int dstNodes = __OCTREE_NODE_TOTAL(static_cast<int>(depthLvl), int);
			oct_tree_node *childAt = m_traversalChildren[srcNode];
			__Insert(this, childAt, geo_obj, &m_traversalChildren[dstNodes], m_traversalChildrenCounts[depthLvl + 1]);
		}
	}

	m_geoList.add(geo_obj);

	return true;
}

bool oct_tree::removeGeometry(geometry2 *geo_obj)
{
	if (!geo_obj)
		throw invalid_argument("'geo_obj' must be a valid pointer");

	return false;
}

void oct_tree::draw()
{
	for (int t = 0; t < m_geoList.count(); ++t)
	{
		m_geoList[t]->draw();
	}
	static oct_tree_node *elem_;
	for (int elem = 0; elem < m_nodeMasterCapacity; ++elem)
	{
		elem_ = *(m_nodeMaster + elem);

		if (elem_)
		{
			Gizmos::addAABB(elem_->m_reg.m_center, elem_->m_reg.m_halfExtents, glm::vec4(1, 1, 0, 1));
			
		}
	}
}

// subdivide an octree node into 8 new children and make the node their parent
void __Subdiv(oct_tree *val0, oct_tree_node *val0Node)
{
	// existing objects must be supplied
	assert(val0 && val0Node);

	// val0Node's level must be at least 1 less than the max depth of the octree
	assert(val0Node->m_depthLevel < val0->m_maxDepth);

	// val0Node mustn't have any children
	assert(val0Node->m_subDepthCount == 0);

	// static variables used below
	static int childX, childY, childZ, index;
	static vec3 val0NodeRegOffset, val0NodeRegSize, childNodeRegSize, childNodeRegOffset;

	// calculate region offset and sizes
	val0NodeRegOffset = val0Node->m_reg.m_min;
	val0NodeRegSize = val0Node->m_reg.m_max - val0NodeRegOffset;
	childNodeRegSize = val0NodeRegSize * 0.5F;

	// reset the index every time '__Subdiv' is successfully run
	index = 0;

	// create each child and parent them
	for (childX = 0; childX < 2; ++childX)
	{
		for (childY = 0; childY < 2; ++childY)
		{
			for (childZ = 0; childZ < 2; ++childZ)
			{
				// calculate child region offset
				childNodeRegOffset = vec3(val0NodeRegOffset + childNodeRegSize * vec3(childX, childY, childZ));

				// create and set up child node
				oct_tree_node *child = new oct_tree_node;
				child->m_container = val0;
				child->m_depthLevel = val0Node->m_depthLevel + 1;
				child->m_reg = region(childNodeRegOffset, childNodeRegOffset + childNodeRegSize);
				child->m_parent = val0Node;
				child->m_index = index;
				child->m_masterIndex = _MasterIndex_(val0Node->m_depthLevel, val0Node->m_index, child->m_index);
				child->m_subDepthCount = 0;

				// parent to val0Node
				val0Node->m_children[index] = child;

				// add to octree master list
				val0->m_nodeMaster[child->m_masterIndex] = child;

				++index;
			}
		}
	}

	// iteratively increment the 'm_subDepthCount' of every parent, grand-parent, great-grand-parent, etc, by 1.
	oct_tree_node *parent_ = val0Node;
	do
	{
		++parent_->m_subDepthCount;

	} while (parent_ = parent_->m_parent);

	// update octree master sparse size
	val0->m_nodeMasterSparseSize += 8; // 8 children added for every subdivide
}

// undoes the effects of __Subdiv(...)
void _Combine(oct_tree *val0, oct_tree_node *val0Node)
{
	// existing objects must be supplied
	assert(val0 && val0Node);

	// val0Node must have at least all direct children and no other descendants
	assert(val0Node->m_subDepthCount == 1);

	// static variables used below
	static int elem;
	static oct_tree_node **elem_;

	// murder the children! >8D
	for (elem = 0; elem < 8; ++elem)
	{
		elem_ = val0Node->m_children + elem;

		if (*elem_)
		{
			// make any geometry2 forget about this node
			__GeomForgetNodes(*elem_);

			// make the master list forget this node
			val0->m_nodeMaster[(*elem_)->m_masterIndex] = nullptr;

			// finally, kill the child and make the parent forget about it. >=;D
			delete *elem_;
			*elem_ = nullptr;
		}
	}

	// iteratively decrement the 'm_subDepthCount' of every parent, grand-parent, great-grand-parent, etc, by 1.
	oct_tree_node *parent_ = val0Node;
	do
	{
		--parent_->m_subDepthCount;

	} while (parent_ = parent_->m_parent);

	// update octree master sparse size
	val0->m_nodeMasterSparseSize -= 8; // 8 children added for every subdivide
}

// calculate an indexing number for the master list given a node's depth level, parent node's index (relative to grand-parent), and the node's index (relative to parent)
inline int _MasterIndex_(byte depth, int parentIndex, int childIndex)
{
	return static_cast<int>(__OCTREE_NODE_TOTAL(static_cast<int>(depth), int) + parentIndex * 8 + childIndex);
}

// run through any geometry2 objects that may reference a node and remove any trace of that node from said geometry2 objects
void __GeomForgetNodes(oct_tree_node *val0)
{
	// static variables used below
	static int index;

	// tell each occupant that we're being deleted
	for (index = val0->m_occupants.count() - 1; index >= 0; --index)
	{
		geometry2 *occup = *val0->m_occupants.data() + index;

		// make sure the occupant exists
		assert(occup);

		// find and delete us from each occupant's list of homes
		int it = occup->m_containingNodes.find(val0);

		// logically we must exist in the occupant's list of homes, otherwise error
		assert(it != -1);

		// finally, remove it from the occupant's list of homes
		occup->m_containingNodes.remove(it);
	}
}

// run through any nodes that may reference a geom and remove any trace of that grom from said nodes
void __NodeForgetGeoms(geometry2 *val0)
{
	// static variables used below
	static int index;

	// tell each home that we're being deleted
	for (index = val0->m_containingNodes.count() - 1; index >= 0; --index)
	{
		 oct_tree_node *node = *val0->m_containingNodes.data() + index;

		// make sure the node exists
		assert(node);

		// find and delete us from each node's list of occupants
		int it = node->m_occupants.find(val0);

		// logically we must exist in the node's list of occupants, otherwise error
		assert(it != -1);

		// finally, remove it from the node's list of occupants
		node->m_occupants.remove(it);
	}
}

// insertion method for a node and may result in a new child depth for that node. Assumes that 'val0Node' intersects with 'val1' already.
// returns if it subdivided or not.
bool __Insert(oct_tree *val0, oct_tree_node *val0Node, geometry2 *val1, oct_tree_node **val2, byte &val2Count)
{
	// static variables used below
	static bool subDivResult, result;
	static int index;
	static oct_tree_node *elem_;

	// existing objects must be supplied
	assert(val0 && val0Node && val1);

	// introduce with potential-parent node assuming the geom makes contact with the node's region
	__Introduce(val1, val0Node);

	// if the node isn't at the max depth, doesn't have children and also needs to split (>1 occupier)
	subDivResult = 
		val0->m_maxDepth > val0Node->m_depthLevel &&
		val0Node->m_subDepthCount == 0 &&
		val0Node->m_occupants.count() > 0;

	if (!subDivResult)
		return false;

	// since we can subdivide, do it
	__Subdiv(val0, val0Node);

	// loop over and check deeper down if the geometry2 intersects each
	for (index = 0; index < 8; ++index)
	{
		elem_ = val0Node->m_children[index];

		// do the intersection
		val1->m_boundingVolume->contact(elem_->m_reg, result);

		// if geom intersected with the child...
		if (result)
		{
			val2[val2Count] = elem_;
			++val2Count;
			printf("New child hit %d\n", val2Count);
		}
	}

	assert(val2Count > 0);

	return true;
}