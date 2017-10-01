#include "my_geoms_bvols.hpp"
#include <stdexcept>
#include "..\Gizmos.h"

using std::invalid_argument;

double SquaredDistPointAABB(const vec3 & p, const region & aabb)
{
	auto check = [&](
		const double pn,
		const double bmin,
		const double bmax) -> double
	{
		double out = 0;
		double v = pn;

		if (v < bmin)
		{
			double val = (bmin - v);
			out += val * val;
		}

		if (v > bmax)
		{
			double val = (v - bmax);
			out += val * val;
		}

		return out;
	};

	// Squared distance
	double sq = 0.0;

	sq += check(p.x, aabb.m_min.x, aabb.m_max.x);
	sq += check(p.y, aabb.m_min.y, aabb.m_max.y);
	sq += check(p.z, aabb.m_min.z, aabb.m_max.z);

	return sq;
}

bounding_volume_aabb::bounding_volume_aabb(const vec3 &extent0, const vec3 &extent1)
{
	m_reg = region(extent0, extent1);
}

inline void bounding_volume_aabb::contact(const region &reg, bool &result)
{
	/*if (abs(m_reg.m_center[0] - reg.m_center[0]) > (m_reg.m_halfExtents[0] + reg.m_halfExtents[0])) { result = false; return; }
	if (abs(m_reg.m_center[1] - reg.m_center[1]) > (m_reg.m_halfExtents[1] + reg.m_halfExtents[1])) { result = false; return; }
	if (abs(m_reg.m_center[2] - reg.m_center[2]) > (m_reg.m_halfExtents[2] + reg.m_halfExtents[2])) { result = false; return; }
	result = true;*/

	result = (
		m_reg.m_max.x > reg.m_min.x &&
		m_reg.m_min.x < reg.m_max.x &&
		m_reg.m_max.y > reg.m_min.y &&
		m_reg.m_min.y < reg.m_max.y &&
		m_reg.m_max.z > reg.m_min.z &&
		m_reg.m_min.z < reg.m_max.z
		);
}
void bounding_volume_aabb::draw()
{
	Gizmos::addAABBFilled(m_reg.m_center, m_reg.m_halfExtents, glm::vec4(1, 0, 0, 1));
}

bounding_volume_sphere::bounding_volume_sphere(const vec3 &center, const float &radius) : m_center(center),
m_radius(radius)
{
}

inline void bounding_volume_sphere::contact(const region &reg, bool &result)
{
	double squaredDistance = SquaredDistPointAABB(m_center, reg);

	// Intersection if the distance from center is larger than the radius
	// of the sphere.

	result = squaredDistance <= (m_radius * m_radius);
}
void bounding_volume_sphere::draw()
{
	Gizmos::addSphere(m_center, m_radius, 8, 16, glm::vec4(1, 0, 0, 1));
}

geometry_sphere::geometry_sphere(const vec3 &center, const float &radius) : geometry2(new bounding_volume_sphere(center, radius)),
	m_center(center), 
	m_radius(radius)
{
	if (radius < 0.0F)
		throw invalid_argument("'radius' must be positive");
}