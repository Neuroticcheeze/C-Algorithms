#include "Entity.hpp"
#include "Octree.hpp"

#include <random>
#include <snow_drop_visualiser.hpp>

Vector3D Entity::MAX_ROAM_AREA(100.0F);

Entity::Entity() :
	position(Vector3D(0.0F)),
	targetpos(Vector3D(0.0F)),
	bounds(AxisAlignedBoundingBox(Vector3D(-0.5F), Vector3D(0.5F)))
{

}

Entity::Entity(const Entity &other) :
	position(other.position),
	targetpos(other.targetpos),
	bounds(other.bounds),
	nodes(other.nodes)
{

}

Entity::Entity(const Vector3D &startpos, const Vector3D &size) :
	position(startpos),
	targetpos(startpos),
	bounds(AxisAlignedBoundingBox(size * -0.5F + startpos, size * 0.5F + startpos))
{

}

void Entity::update(const float &dt, bool repel)
{
	life += dt;

#define ENTITY_JITTER
#ifdef ENTITY_JITTER
	if (life > (static_cast<float>(rand() % RAND_MAX) / RAND_MAX) * 2.1F)
	{
		life = 0;

		if (nodes.size() > 0)
		{
			int i = rand() % nodes.size();
			int j = rand() % nodes[i]->population.size();
			Entity *ent = nodes[i]->population[j];

			if (repel)
			{
				targetpos = position + Vector3D::random(Vector3D(2000.0F));
			}

			else
			{
				targetpos = ent->position + Vector3D::random(Vector3D(2.0F));
			}
		}

		else
		{
			targetpos = Vector3D::random(MAX_ROAM_AREA * 4.0F);
		}
	}
#endif

	position += (targetpos - position) * 0.05F * dt;
	bounds.setCenter(position);
}

AxisAlignedBoundingBox Entity::getBounds() const
{
	return bounds;
}