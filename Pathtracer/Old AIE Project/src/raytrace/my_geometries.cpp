#include "my_geometries.hpp"

//SPHERE
sphere::sphere(const vec3 &pos, const float &radius) : geometry(geometry::sphere),
	pos(pos), radius(radius) 
{

}
//bool sphere::test(const ray &ray_, hit_info &result) const
//{
//	vec3 rayPos__ = ray_.pos;
//	vec3 rayDir__ = ray_.dir;
//
//	vec3 I = rayPos__;
//	vec3 n = rayDir__;
//	vec3 P = this->pos;
//	float r = this->radius;
//
//	float b = dot(n, I - P);
//	vec3 diff = I - P;
//	float c = dot(diff, diff) - (r * r);
//	float d = b * b - c;
//
//	if (d < 0.0F) return false;
//
//	float t = -(b + sqrt(d));
//
//	//Set up the contact result profile
//	result.hit_position = I + n * t;
//	result.hit_normal = normalize(result.hit_position - P);
//	result.hit_tangent = cross(vec3(0.0F, 1.0F, 0.0F), result.hit_normal);
//	result.incidence_length = t;
//	result.tex_coord = vec2(asin(result.hit_normal.x) / 3.14159F + 0.5F, asin(result.hit_normal.y) / 3.14159F + 0.5F);
//
//	return true;
//}
bool sphere::test(const ray &ray_, hit_info &result) const
{
	vec3 rayPos__ = ray_.pos;
	vec3 rayDir__ = ray_.dir;

	vec3 I = rayPos__;
	vec3 n = rayDir__;
	vec3 P = this->pos;
	float r = this->radius;

	float d;
	if (!glm::intersectRaySphere(I, n, P, r * r, d))
		return false;



	//Set up the contact result profile
	result.hit_position = I + n * d;
	result.hit_normal = normalize(result.hit_position - P);
	result.hit_tangent = cross(vec3(0.0F, 1.0F, 0.0F), result.hit_normal);
	result.incidence_length = d;
	result.tex_coord = vec2(asin(result.hit_normal.x) / 3.14159F + 0.5F, asin(result.hit_normal.y) / 3.14159F + 0.5F);

	return true;
}

//TRIANGLE
triangle::triangle(const triangle_vertex &vert0, const triangle_vertex &vert1, const triangle_vertex &vert2) : geometry(geometry::triangle),
	vert0(vert0), vert1(vert1), vert2(vert2)
{

}
bool triangle::test(const ray &ray_, hit_info &result) const
{
	vec3 outPos;
	if (glm::intersectLineTriangle(ray_.pos, ray_.dir, this->vert0.pos, this->vert1.pos, this->vert2.pos, outPos))
	{
		//Set up the contact result profile
		result.hit_position = outPos;
		result.hit_normal = vec3(0, 1, 0);// normalize(result.hit_position - P);
		result.hit_tangent = vec3(1, 0, 0);// cross(vec3(0.0F, 1.0F, 0.0F), result.hit_normal);
		result.incidence_length = glm::distance(outPos, ray_.pos);
		//result.tex_coord = vec2(asin(result.hit_normal.x) / 3.14159F + 0.5F, asin(result.hit_normal.y) / 3.14159F + 0.5F);
		return true;
	}

	return false;
}

plane::plane(const vec3 &pos, const vec3 &normal, const vec3 &tangent) : geometry(geometry::plane),
	pos(pos),
	normal(glm::normalize(normal)),
	tangent(glm::normalize(tangent))
{
	if (this->tangent.x == 0 && this->tangent.y == 0 && this->tangent.z == 0)
	{
		this->tangent = glm::cross(vec3(0, 1, 0), normal);
	}

	if (this->tangent.x == 0 && this->tangent.y == 0 && this->tangent.z == 0)
	{
		this->tangent = glm::cross(vec3(0, 0, 1), normal);
	}
}
bool plane::test(const ray &ray_, hit_info &result) const
{
	float dist;
	vec3 I = ray_.pos;
	vec3 n = ray_.dir;

	if (glm::intersectRayPlane(I, n, pos, normal, dist))
	{
		//Set up the contact result profile
		result.hit_position = I + n * dist;
		result.hit_normal = normal;
		result.hit_tangent = tangent;
		result.incidence_length = dist;
		//result.tex_coord = ;

		return true;
	}

	return false;
}