#include "Geometry.hpp"
#include "Entity.hpp"
#include "Octree.hpp"

#include <snow_drop_visualiser.hpp>
#include <imgui.h>
#include <vector>

#define ENTITY_COUNT 12
#define SUBDIV_LIMIT 8
#define CLUTTER_LIMIT 4
#define SHRINK_WRAP
#define USE_SPACIAL_PARTITIONING


Entity entities[ENTITY_COUNT];
const int entitiesCount = sizeof(entities) / sizeof(Entity);

void DrawAABB(const AxisAlignedBoundingBox & aabb, bool flag)
{
	sdvGraphics::addAABBFilled(aabb.getCenter().vals, aabb.getExtents().vals, Colour3F(!flag, flag, 0.0F));
}

void app(const float *positionVec, const float *forwardVec, const float *upVec, const float *rightVec, const float &deltaTime)
{
#ifdef USE_SPACIAL_PARTITIONING
#ifdef SHRINK_WRAP
	Vector3D maxmax(-FLT_MAX + FLT_EPSILON), minmin(FLT_MAX);
	for (int i = 0; i < entitiesCount; ++i) { auto ent = &entities[i]; minmin = Vector3D::min(minmin, ent->getBounds().getMin()); maxmax = Vector3D::max(maxmax, ent->getBounds().getMax()); }
	Octree *octree = Octree::Build(AxisAlignedBoundingBox(minmin, maxmax), SUBDIV_LIMIT, CLUTTER_LIMIT, entities, entitiesCount);
#else
	Octree *octree = Octree::Build(AxisAlignedBoundingBox(Entity::MAX_ROAM_AREA * -1, Entity::MAX_ROAM_AREA), SUBDIV_LIMIT, CLUTTER_LIMIT, entities, entitiesCount);
#endif
	Octree::ForEach(octree, [](const Node *node) {
		sdvGraphics::addAABB(node->bounds.getCenter().vals, (node->bounds.getExtents()).vals, Colour4F(0,1,0,0.2F));
	});
#endif

	static bool repel = false;
	static float dtScale = 1.0F;
	ImGui::SliderFloat("Time Scale", &dtScale, 0.0F, 20.0F);
	ImGui::Checkbox("Repel Eachother", &repel);

	for (int i = 0; i < entitiesCount; ++i)
	{
		auto entity = &entities[i];
		entity->update(deltaTime * dtScale, repel);

		bool collided = false;

#ifdef USE_SPACIAL_PARTITIONING
#pragma omp parallel for reduction(+:x,y)
		for (Node *node : entity->nodes)
		{
			if (collided == false && node && node->children[0] == nullptr)
			for (Entity *ent : node->population)
			{
				if (ent == entity) continue;

				if (entity->getBounds().intersects(ent->getBounds()))
				{
					collided = true;
					break;
				}
			}
		}
#else
		for (int i2 = 0; i2 < entitiesCount; ++i2)
		{
			if (i == i2) continue;

			auto &entity2 = entities[i2];
			if (entity->getBounds().intersects(entity2.getBounds()))
			{
				collided = true;
				break;
			}
		}
#endif
		DrawAABB(entity->getBounds(), collided);
	}

#ifdef USE_SPACIAL_PARTITIONING
	delete octree;
#endif
}

void main()
{
	for (int i = 0; i < entitiesCount; ++i)
	{
		entities[i] = Entity(Vector3D::random(Entity::MAX_ROAM_AREA), Vector3D::random(Vector3D(5.0F)) + 0.5F);
	}

	sdvContext *ctx = new sdvContext("Octree App", 1200, 960);
	ctx->hookUpdate(app);
	ctx->start();
	delete ctx;
}