#include "Vector4.h"

#pragma once

#include "Common.h"

namespace Math
{

	Vector4::Vector4(void) :
		x(0.0f), y(0.0f), z(0.0f), w(0.0f)
	{
	}

	Vector4::Vector4(float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = 0;
	}

	Vector4::Vector4(float x, float y, float z, float w)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}

	Vector4::~Vector4(void)
	{

	}

	Vector4 Vector4::operator - (const Vector4& a) const
	{
		return Vector4(x-a.x, y-a.y, z-a.z, w-a.w);
	}

	Vector4 Vector4::operator + (const Vector4& a) const
	{
		return Vector4( a.x + x,
			a.y + y,
			a.z + z,
			a.w + w);
	}

	bool Vector4::operator == (const Vector4& rhs) const
	{
		const float kEpsilon = 1e-005f;
		bool x = abs(this->x - rhs.x) < kEpsilon;
		bool y = abs(this->y - rhs.y) < kEpsilon;
		bool z = abs(this->z - rhs.z) < kEpsilon;
		bool w = abs(this->w - rhs.w) < kEpsilon;

		return x & y & z & w;
	}

	bool Vector4::operator != (const Vector4& rhs) const
	{
		return !Vector4::operator==(rhs);
	}

	Vector4 Vector4::operator * (float d) const
	{
		return Vector4(x * d, y * d, z * d, w * d);
	}

	Vector4 Vector4::operator * (const Vector4& a) const
	{
		return Vector4(a.x * x, a.y * y, a.z * z, a.w * w);
	}

	Vector4 Vector4::operator / (float d) const
	{
		return Vector4(x/d, y/d, z/d, w/d);
	}

	Vector4 Vector4::One()
	{
		return Vector4(1, 1, 1, 1);
	}

	Vector4 Vector4::Zero()
	{
		return Vector4(0, 0, 0, 0);
	}

	float Vector4::Distance(const Vector4& a, const Vector4& b)
	{
		float sx = (b.x - a.x) * (b.x - a.x);
		float sy = (b.y - a.y) * (b.y - a.y);
		float sz = (b.z - a.z) * (b.z - a.z);
		float sw = (b.w - a.w) * (b.w - a.w);

		return sqrtf(sx+sy+sz+sw);
	}

	float Vector4::Dot(const Vector4& lhs, const Vector4& rhs)
	{
		return (lhs.x * rhs.x) + (lhs.y * rhs.y) + (lhs.z * rhs.z) + (lhs.w * rhs.w);
	}

	Vector4 Vector4::Lerp(const Vector4& from, const Vector4& to, float t)
	{ 
		Vector4 a = to - from;
		return a * t;
	}

	Vector4 Vector4::Min(const Vector4& lhs, const Vector4& rhs)
	{
		float x = lhs.x > rhs.x ? rhs.x : lhs.x;
		float y = lhs.y > rhs.y ? rhs.y : lhs.y;
		float z = lhs.z > rhs.z ? rhs.z : lhs.z;
		float w = lhs.w > rhs.w ? rhs.w : lhs.w;

		return Vector4(x, y, z, w);
	}

	Vector4 Vector4::Max(const Vector4& lhs, const Vector4& rhs)
	{
		float x = lhs.x > rhs.x ? lhs.x : rhs.x;
		float y = lhs.y > rhs.y ? lhs.y : rhs.y;
		float z = lhs.z > rhs.z ? lhs.z : rhs.z;
		float w = lhs.w > rhs.w ? lhs.w : rhs.w;

		return Vector4(x, y, z, w);
	}

	float Vector4::Legnth(const Vector4& a)
	{
		return sqrtf((a.x * a.x) + (a.y + a.y) + (a.z * a.z) + (a.w * a.w));
	}

	Vector4 Vector4::Normalize(Vector4& value)
	{
		float v = Vector4::Legnth(value);
		return value / v;
	}

	Vector4 Vector4::Scale(const Vector4& a, const Vector4& b)
	{
		return Vector4(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
	}

	float Vector4::SqrLegnth(const Vector4& a)
	{
		return (a.x * a.x) + (a.y + a.y) + (a.z * a.z) + (a.w * a.w);
	}

	void Vector4::Normalize()
	{
		float v = Vector4::Legnth(*this);
		*this = *this / v;
	}

	void Vector4::Scale(const Vector4& scale)
	{
		x *= scale.x;
		y *= scale.y;
		z *= scale.z;
		w *= scale.w;
	}

	void Vector4::Set(float newX, float newY, float newZ, float newW)
	{
		x = newX;
		y = newY;
		z = newZ;
		w = newW;
	}

}