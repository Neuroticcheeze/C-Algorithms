#pragma once
#ifndef RAYTRACE_H_
#define RAYTRACE_H_

#include "colour.hpp"
#include <glm\common.hpp>
#include <glm\mat4x4.hpp>
#include <glm\matrix.hpp>
#include <glm\ext.hpp>
#include <vector>
using glm::uvec2;
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;
using glm::normalize;
using glm::lookAt;
using glm::perspective;
using glm::inverse;

struct ray
{
	ray(const vec3 &pos = vec3(), const vec3 &dir = vec3()) : pos(pos), dir(normalize(dir)) {}

	vec3 pos, dir;
};

struct camera
{
	friend void drawSceneGeometryAndCamera(const camera &camera);

	camera() {}
	camera(const vec3 &pos, const vec3 &dir, float fov, float near_, float far_);
	ray project(unsigned x, unsigned y);

private:
	mat4 invProj;
	vec3 pos, dir;
	float fov, near__, far__;
};

struct material
{
	enum type
	{
		metal,
		glowy,
	};
	material() : material(metal, colour()) {}
	material(type mattype, colour col) : _type(mattype), _col(col) {}
	type _type;
	colour _col;
};

struct hit_info;

struct geometry
{
	enum type
	{
		sphere,
		triangle,
		plane,
	};

	geometry(type type__) { type_ = type__; }
	virtual bool test(const ray &ray_, hit_info &result) const = 0;

	geometry *setMaterial(material mat)
	{
		_material = mat;
		return this;
	}

	material _material;
	type type_;
};

struct hit_info
{
	enum termination_type
	{
		emissive_hit,
		reflective_hit,
		background_hit,
	};

	//Non termination data
	vec3 hit_position;
	vec3 hit_normal;
	vec3 hit_tangent;
	float incidence_length;
	vec2 tex_coord;
	//////////////////////

	geometry *potentialHitGeom = nullptr;

	//How the ray was terminated
	termination_type rayTerm;
};

struct ray_tree_node
{
	//Index of the previous node that this is a sample to. If -1, then 'incident' ray is straight from the camera.
	int prev;

	//The incidence ray.
	ray incident;

	//Information about the ray-geom intersection
	hit_info inf;

	//An index pointer to the evaluated colour from this node's samples. If it's -1, then this node terminated on the skybox or a black body or emissive material.
	int evalColourPtr = -1;
};

struct ray_tree
{
	int bounce;
	std::vector<ray_tree_node*> *data = nullptr;//The array of vectors equals the max bounce count.
	std::vector<uvec2> *sampleRanges = nullptr;//To track where sample groupings are on each bounce row.
};

extern void initialiseRaytracer(unsigned bounces, unsigned samples);

extern void addGeometry(geometry *geom);

extern bool testScene(const ray &ray_, const geometry *previousCollision, hit_info &result);

extern bool buildRayTree(const ray &initialCameraRay, ray_tree *rayTree);

extern vec4 evaluateRayTree(ray_tree *rayTree);

extern void deleteRayTree(ray_tree **tree);

extern geometry *getGeometry(int index);

extern void finaliseRaytracer();

extern void drawSceneGeometryAndCamera(const camera &camera);
extern void drawRayTree(const ray_tree *rayTree);

#endif//RAYTRACE_H_