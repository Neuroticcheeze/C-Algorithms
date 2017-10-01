#pragma once
#ifndef _H_GEOMETRY
#define _H_GEOMETRY

#define Colour3F(r, g, b) Vector3D( r, g, b, 1.0F ).vals
#define Colour4F(r, g, b, a) Vector3D( r, g, b, a ).vals

//Cartesian based vector in 3D space.
struct Vector3D
{

	union {
		struct { float r, g, b, a; };
		struct { float x, y, z; };
		float vals[4];
	};

	Vector3D();
	Vector3D(const Vector3D &other);
	explicit Vector3D(const float &scalar);
	Vector3D(const float &x, const float &y);
	Vector3D(const float &x, const float &y, const float &z);
	Vector3D(const float &r, const float &g, const float &b, const float &a);
	inline bool operator==(const Vector3D &rhs) const;
	inline bool operator!=(const Vector3D &rhs) const;
	inline bool operator<(const Vector3D &rhs) const;
	inline bool operator>(const Vector3D &rhs) const;
	inline bool operator<=(const Vector3D &rhs) const;
	inline bool operator>=(const Vector3D &rhs) const;
	inline bool operator<(const float &rhs) const;
	inline bool operator>(const float &rhs) const;
	inline bool operator<=(const float &rhs) const;
	inline bool operator>=(const float &rhs) const;
	inline Vector3D operator-() const;
	Vector3D &operator=(const Vector3D &rhs);
	friend Vector3D operator+(const Vector3D &lhs, const Vector3D &rhs);
	friend Vector3D operator+(const Vector3D &lhs, const float &rhs);
	friend Vector3D operator-(const Vector3D &lhs, const Vector3D &rhs);
	friend Vector3D operator-(const Vector3D &lhs, const float &rhs);
	friend Vector3D operator*(const Vector3D &lhs, const Vector3D &rhs);
	friend Vector3D operator*(const Vector3D &lhs, const float &rhs);
	friend Vector3D operator/(const Vector3D &lhs, const Vector3D &rhs);
	friend Vector3D operator/(const Vector3D &lhs, const float &rhs);
	Vector3D &operator+=(const Vector3D &rhs);
	Vector3D &operator+=(const float &rhs);
	Vector3D &operator-=(const Vector3D &rhs);
	Vector3D &operator-=(const float &rhs);
	Vector3D &operator*=(const Vector3D &rhs);
	Vector3D &operator*=(const float &rhs);
	Vector3D &operator/=(const Vector3D &rhs);
	Vector3D &operator/=(const float &rhs);
	float length() const;
	float lengthSqr() const;
	Vector3D normalised() const;
	inline float max() const;
	inline float min() const;
	static float distance(const Vector3D &lhs, const Vector3D &rhs);
	static float distanceSqr(const Vector3D &lhs, const Vector3D &rhs);
	static float dot(const Vector3D &lhs, const Vector3D &rhs);
	static Vector3D cross(const Vector3D &lhs, const Vector3D &rhs);
	static Vector3D max(const Vector3D &lhs, const Vector3D &rhs);
	static Vector3D min(const Vector3D &lhs, const Vector3D &rhs);
	static Vector3D random(const Vector3D &extents);
};

//Axis Aligned Bounding Box in 3D space, defined by min and max anchors.
struct AxisAlignedBoundingBox
{
private:
	//The anchors
	Vector3D min, max;

public:
	AxisAlignedBoundingBox();
	AxisAlignedBoundingBox(const AxisAlignedBoundingBox &other);
	AxisAlignedBoundingBox(const Vector3D &min, const Vector3D &max);

	inline bool operator==(const AxisAlignedBoundingBox &rhs) const;
	inline bool operator!=(const AxisAlignedBoundingBox &rhs) const;
	bool intersects(const Vector3D &rhs);
	bool intersects(const AxisAlignedBoundingBox &rhs);

	void setCenter(const Vector3D &pos);
	void setExtents(const Vector3D &ext);
	void setMin(const Vector3D &value);
	void setMax(const Vector3D &value);
	Vector3D getCenter() const;
	Vector3D getExtents() const;
	Vector3D getMin() const;
	Vector3D getMax() const;

private:
	void refresh();
};

namespace Collisions
{
	// Checks if a point lies in an AABB.
	bool Contains(const Vector3D &min, const Vector3D &max, const Vector3D &p);

	// Uses the Minkowski Difference to determine if two AABBs overlap in any way.
	bool Overlaps(const Vector3D &min0, const Vector3D &max0, const Vector3D &min1, const Vector3D &max1);
}

#endif//_H_GEOMETRY