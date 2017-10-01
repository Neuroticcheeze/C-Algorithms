#include "Geometry.hpp"
#include <limits>

#define FLT_CMP(a, b) (fabs(a - b) < FLT_EPSILON)
#define MIN(a, b) (a < b ? a : b)
#define MAX(a, b) (a > b ? a : b)

#pragma region Vector3D
Vector3D::Vector3D() :
	x(0.0F), y(0.0F), z(0.0F)
{}

Vector3D::Vector3D(const Vector3D &other) :
	x(other.x), y(other.y), z(other.z)
{}

Vector3D::Vector3D(const float &scalar) :
	x(scalar), y(scalar), z(scalar)
{}

Vector3D::Vector3D(const float &x, const float &y) :
	x(x), y(y), z(0.0F)
{}

Vector3D::Vector3D(const float &x, const float &y, const float &z) :
	x(x), y(y), z(z)
{}

Vector3D::Vector3D(const float &r, const float &g, const float &b, const float &a) :
	r(r), g(g), b(b), a(a)
{}

bool Vector3D::operator==(const Vector3D &rhs) const
{
	return FLT_CMP(rhs.x, x) && FLT_CMP(rhs.y, y) && FLT_CMP(rhs.z, z);
}

bool Vector3D::operator!=(const Vector3D &rhs) const
{
	return !FLT_CMP(rhs.x, x) || !FLT_CMP(rhs.y, y) || !FLT_CMP(rhs.z, z);
}

bool Vector3D::operator<(const Vector3D &rhs) const
{
	return x < rhs.x && y < rhs.y && z < rhs.z;
}

bool Vector3D::operator>(const Vector3D &rhs) const
{
	return x > rhs.x && y > rhs.y && z > rhs.z;
}

bool Vector3D::operator<=(const Vector3D &rhs) const
{
	return x <= rhs.x && y <= rhs.y && z <= rhs.z;
}

bool Vector3D::operator>=(const Vector3D &rhs) const
{
	return x >= rhs.x && y >= rhs.y && z >= rhs.z;
}

bool Vector3D::operator<(const float &rhs) const
{
	return x < rhs && y < rhs && z < rhs;
}

bool Vector3D::operator>(const float &rhs) const
{
	return x > rhs && y > rhs && z > rhs;
}

bool Vector3D::operator<=(const float &rhs) const
{
	return x <= rhs && y <= rhs && z <= rhs;
}

bool Vector3D::operator>=(const float &rhs) const
{
	return x >= rhs && y >= rhs && z >= rhs;
}

Vector3D Vector3D::operator-() const
{
	return Vector3D(-x, -y, -z);
}

Vector3D &Vector3D::operator=(const Vector3D &rhs)
{
	x = rhs.x;
	y = rhs.y;
	z = rhs.z;
	return *this;
}

Vector3D operator+(const Vector3D &lhs, const Vector3D &rhs)
{
	return Vector3D(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
}

Vector3D operator+(const Vector3D &lhs, const float &rhs)
{
	return Vector3D(lhs.x + rhs, lhs.y + rhs, lhs.z + rhs);
}

Vector3D operator-(const Vector3D &lhs, const Vector3D &rhs)
{
	return Vector3D(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
}

Vector3D operator-(const Vector3D &lhs, const float &rhs)
{
	return Vector3D(lhs.x - rhs, lhs.y - rhs, lhs.z - rhs);
}

Vector3D operator*(const Vector3D &lhs, const Vector3D &rhs)
{
	return Vector3D(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z);
}

Vector3D operator*(const Vector3D &lhs, const float &rhs)
{
	return Vector3D(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs);
}

Vector3D operator/(const Vector3D &lhs, const Vector3D &rhs)
{
	return Vector3D(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z);
}

Vector3D operator/(const Vector3D &lhs, const float &rhs)
{
	return Vector3D(lhs.x / rhs, lhs.y / rhs, lhs.z / rhs);
}

Vector3D &Vector3D::operator+=(const Vector3D &rhs)
{
	*this = *this + rhs;
	return *this;
}

Vector3D &Vector3D::operator+=(const float &rhs)
{
	*this = *this + rhs;
	return *this;
}

Vector3D &Vector3D::operator-=(const Vector3D &rhs)
{
	*this = *this - rhs;
	return *this;
}

Vector3D &Vector3D::operator-=(const float &rhs)
{
	*this = *this - rhs;
	return *this;
}

Vector3D &Vector3D::operator*=(const Vector3D &rhs)
{
	*this = *this * rhs;
	return *this;
}

Vector3D &Vector3D::operator*=(const float &rhs)
{
	*this = *this * rhs;
	return *this;
}

Vector3D &Vector3D::operator/=(const Vector3D &rhs)
{
	*this = *this / rhs;
	return *this;
}

Vector3D &Vector3D::operator/=(const float &rhs)
{
	*this = *this / rhs;
	return *this;
}

float Vector3D::length() const
{
	return sqrtf(x * x + y * y + z * z);
}

float Vector3D::lengthSqr() const
{
	return x * x + y * y + z * z;
}

Vector3D Vector3D::normalised() const
{
	return Vector3D(*this) / length();
}

float Vector3D::max() const
{
	return (x > y) ? ((x > z) ? (x) : (z)) : ((y > z) ? (y) : (z));
}

float Vector3D::min() const
{
	return (x < y) ? ((x < z) ? (x) : (z)) : ((y < z) ? (y) : (z));
}

float Vector3D::distance(const Vector3D &lhs, const Vector3D &rhs)
{
	return (lhs - rhs).length();
}

float Vector3D::distanceSqr(const Vector3D &lhs, const Vector3D &rhs)
{
	return (lhs - rhs).lengthSqr();
}

float Vector3D::dot(const Vector3D &lhs, const Vector3D &rhs)
{
	return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

Vector3D Vector3D::cross(const Vector3D &lhs, const Vector3D &rhs)
{
	return Vector3D(
		lhs.y * rhs.z - lhs.z * rhs.y, 
		lhs.z * rhs.x - lhs.x * rhs.z, 
		lhs.x * rhs.y - lhs.y * rhs.x);
}

Vector3D Vector3D::max(const Vector3D &lhs, const Vector3D &rhs)
{
	return Vector3D(MAX(lhs.x, rhs.x), MAX(lhs.y, rhs.y), MAX(lhs.z, rhs.z));
}

Vector3D Vector3D::min(const Vector3D &lhs, const Vector3D &rhs)
{
	return Vector3D(MIN(lhs.x, rhs.x), MIN(lhs.y, rhs.y), MIN(lhs.z, rhs.z));
}

Vector3D Vector3D::random(const Vector3D &extents)
{
	float xx = static_cast<float>(rand() % RAND_MAX) / RAND_MAX;
	float yy = static_cast<float>(rand() % RAND_MAX) / RAND_MAX;
	float zz = static_cast<float>(rand() % RAND_MAX) / RAND_MAX;

	return (Vector3D(xx, yy, zz) * 2.0F - 1.0F) * extents;
}
#pragma endregion//Vector3D

#pragma region AxisAlignedBoundingBox
AxisAlignedBoundingBox::AxisAlignedBoundingBox() :
	min(0.0F), max(0.0F)
{}

AxisAlignedBoundingBox::AxisAlignedBoundingBox(const AxisAlignedBoundingBox &other) :
	min(other.min), max(other.max)
{
	refresh();
}

AxisAlignedBoundingBox::AxisAlignedBoundingBox(const Vector3D &min, const Vector3D &max) :
	min(min), max(max)
{
	refresh();
}

bool AxisAlignedBoundingBox::operator==(const AxisAlignedBoundingBox &rhs) const
{
	return min == rhs.min && max == rhs.max;
}

bool AxisAlignedBoundingBox::operator!=(const AxisAlignedBoundingBox &rhs) const
{
	return min != rhs.min || max != rhs.max;
}

bool AxisAlignedBoundingBox::intersects(const Vector3D &rhs)
{
	return Collisions::Contains(min, max, rhs);
}

bool AxisAlignedBoundingBox::intersects(const AxisAlignedBoundingBox &rhs)
{
	return Collisions::Overlaps(min, max, rhs.min, rhs.max);
}

void AxisAlignedBoundingBox::setCenter(const Vector3D &pos)
{
	Vector3D extents = getExtents();

	min = pos - extents;
	max = pos + extents;

	refresh();
}

void AxisAlignedBoundingBox::setExtents(const Vector3D &ext)
{
	Vector3D center = getCenter();

	min = center - ext;
	max = center + ext;

	refresh();
}

void AxisAlignedBoundingBox::setMin(const Vector3D &value)
{
	min = value;
	refresh();
}

void AxisAlignedBoundingBox::setMax(const Vector3D &value)
{
	max = value;
	refresh();
}

Vector3D AxisAlignedBoundingBox::getCenter() const
{
	return (max + min) * 0.5F;
}

Vector3D AxisAlignedBoundingBox::getExtents() const
{
	return (max - min) * 0.5F;
}

Vector3D AxisAlignedBoundingBox::getMin() const
{
	return min;
}

Vector3D AxisAlignedBoundingBox::getMax() const
{
	return max;
}

void AxisAlignedBoundingBox::refresh()
{
	Vector3D a = min;
	Vector3D b = max;

	min = Vector3D::min(a, b);
	max = Vector3D::max(a, b);
}

#pragma endregion//AxisAlignedBoundingBox

#pragma region Collisions
bool Collisions::Contains(const Vector3D &min, const Vector3D &max, const Vector3D &p)
{
	return p >= min && p <= max;
}

bool Collisions::Overlaps(const Vector3D &min0, const Vector3D &max0, const Vector3D &min1, const Vector3D &max1)
{
	Vector3D c0 = (min0 + max0) / 2.0F;
	Vector3D c1 = (min1 + max1) / 2.0F;
	Vector3D e0 = (max0 - c0);
	Vector3D e1 = (max1 - c1);

	e1 += e0;

	Vector3D _min1, _max1;

	_min1 = c1 - e1;
	_max1 = c1 + e1;
	
	return Contains(_min1, _max1, c0);
}
#pragma endregion//Collisions