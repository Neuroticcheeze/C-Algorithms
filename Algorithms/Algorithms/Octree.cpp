#include "Octree.hpp"
 
#define PER_CHILD_ for (int i = 0; i < 8; ++i)

#pragma region Node
Node::Node()
{
	if (children == nullptr) children = new Node*[8];
	memset(children, 0, 8 * sizeof(Node*));
}

Node::Node(const Node& other)
{
	*this = other;
}

Node::~Node()
{
	PER_CHILD_ delete children[i];
	delete[] children;
}

Node& Node::operator=(const Node& other)
{
	this->population = other.population;
	this->bounds = other.bounds;

	if (children == nullptr) children = new Node*[8];
	PER_CHILD_
	{
		children[i] = other.children[i] ? new Node(*other.children[i]) : nullptr;
	}

	return *this;
}
#pragma endregion

#pragma region Octree
void split_node(Node *node, int clutter, int rem);
void per_children(Node *node, fn_perNode action);

Octree::Octree() :
	root(nullptr)
{

}

Octree::Octree(const Octree& other)
{
	*this = other;
}

Octree::~Octree() noexcept
{
	if (root != nullptr) delete root;
}

Octree& Octree::operator=(const Octree& other)
{
	root = other.root ? new Node(*other.root) : nullptr;

	return *this;
}

Octree* Octree::Build(AxisAlignedBoundingBox volume, int maxSubdivs, int maxClutter, Entity *entities, int entitiesCount)
{
	Octree *ot = new Octree();
	ot->root = new Node();
	ot->root->bounds = volume;
	ot->root->population.resize(entitiesCount);
	for (int s = 0; s < entitiesCount; ++s)
	{
		ot->root->population[s] = entities + s;
		ot->root->population[s]->nodes.clear();
		ot->root->population[s]->nodes.push_back(ot->root);
	}

	if (ot->root->population.size() > maxClutter)
	{
		split_node(ot->root, maxClutter, maxSubdivs - 1);
	}

	return ot;
}

void Octree::ForEach(const Octree *octree, fn_perNode action)
{
	per_children(octree->root, action);
}

const Vector3D flipX(-1.0F, 1.0F, 1.0F);
const Vector3D flipY(1.0F, -1.0F, 1.0F);
const Vector3D flipZ(1.0F, 1.0F, -1.0F);

void split_node(Node *node, int clutter, int rem)
{
	if (rem <= 0)
		return;

	if (node)
	{
		auto bounds = node->bounds;
		auto e__ = bounds.getExtents();
		auto enx = e__ * flipX;
		auto eny = e__ * flipY;
		auto enz = e__ * flipZ;
		auto c = bounds.getCenter();

		AxisAlignedBoundingBox subbounds[8] =
		{
			AxisAlignedBoundingBox(c, c - e__),
			AxisAlignedBoundingBox(c, c + e__),
			AxisAlignedBoundingBox(c, c - enx),
			AxisAlignedBoundingBox(c, c + enx),
			AxisAlignedBoundingBox(c, c - eny),
			AxisAlignedBoundingBox(c, c + eny),
			AxisAlignedBoundingBox(c, c - enz),
			AxisAlignedBoundingBox(c, c + enz),
		};

		PER_CHILD_
		{
			node->children[i] = new Node();
			node->children[i]->bounds = subbounds[i];

			for (Entity * ent : node->population)
			{
				if (ent->getBounds().intersects(node->children[i]->bounds))
				{
					ent->nodes.push_back(node->children[i]);
					node->children[i]->population.push_back(ent);
				}
			}

			if (node->children[i]->population.size() > clutter)
			{
				split_node(node->children[i], clutter, rem - 1);
			}
		}
	}
}

void per_children(Node *node, fn_perNode action)
{
	if (node != nullptr)
	{
		action(node);

		for (int n = 0; n < 8; ++n)
		{
			Node *ch = node->children[n];

			if (ch != nullptr)
			{
				per_children(ch, action);
			}
		}
	}
}
#pragma endregion