#include "Vector2.h"
#include <float.h>
#include <math.h>
#include "Common.h"

using namespace Math;

Vector2::Vector2(float _x, float _y) : x(_x), y(_y)
{
}

const Vector2 Vector2::operator + () const
{
	return (*this);
}

const Vector2 Vector2::operator - () const
{
	return Vector2(-x, -y);
}

const Vector2 Vector2::operator - (const Vector2 &a) const
{
	return Vector2(x - a.x, y - a.y);
}

const Vector2 Vector2::operator + (const Vector2 &a) const
{
	return Vector2(a.x + x, a.y + y);
}

bool Vector2::operator == (const Vector2 b) const
{
	return (fabsf(x - b.x) < FLT_EPSILON) && (fabsf(y - b.y) < FLT_EPSILON);
}

bool Vector2::operator != (const Vector2 &b) const
{
	return !Vector2::operator==(b);
}

const Vector2 Vector2::operator * (float d) const
{
	return Vector2(x * d, y * d);
}

const Vector2 Vector2::operator * (const Vector2 &a) const
{
	return Vector2(a.x * x, a.y * y);
}

float Vector2::operator[](unsigned int idx) const
{
	return value[idx];
}

const Vector2 Vector2::operator / (float d) const
{
	return Vector2(x / d, y / d);
}

const Vector2 Vector2::Zero()
{
	return Vector2(0, 0);
}

const Vector2 Vector2::One()
{
	return Vector2(1, 1);
}

float Vector2::AnglebyPoint(const Vector2 &from, const Vector2 &to, bool radian)
{
	float x = to.x - from.x;
	float y = to.y - from.y;

	return radian ? atan2f(y, x) : RAD_TO_DEG(atan2f(y, x));
}

float Vector2::AnglebyDirect(const Vector2 &from, const Vector2 &to, bool radian)
{
	return radian ? acosf(Vector2::Dot(from, to)) : RAD_TO_DEG(acosf(Vector2::Dot(from, to)));
}

float Vector2::Distance(const Vector2 &a, const Vector2 &b)
{
	float sx = (b.x - a.x) * (b.x - a.x);
	float sy = (b.y - a.y) * (b.y - a.y);

	return sqrtf(sx + sy);
}

float Vector2::Dot(const Vector2 &a, const Vector2 &b)
{
	return (a.x * b.x) + (a.y * b.y);
}

const Vector2 Vector2::Lerp(const Vector2 &from, const Vector2 &to, float t)
{
	Vector2 a = to - from;
	return a * t;
}

const Vector2 Vector2::Min(const Vector2 &a, const Vector2 &b)
{
	float x = a.x > b.x ? b.x : a.x;
	float y = a.y > b.y ? b.y : a.y;

	return Vector2(x, y);
}

const Vector2 Vector2::Max(const Vector2 &a, const Vector2 &b)
{
	float x = a.x > b.x ? a.x : b.x;
	float y = a.y > b.y ? a.y : b.y;

	return Vector2(x, y);
}

float Vector2::Length(const Vector2 &a)
{
	return sqrtf((a.x * a.x) + (a.y + a.y));
}

const Vector2 Vector2::Normalize(const Vector2 &value)
{
	float v = Vector2::Length(value);
	return value / v;
}

float Vector2::SqrLength(const Vector2 &a)
{
	return (a.x * a.x) + (a.y + a.y);
}

const Vector2 Vector2::Normalized() const
{
	return Vector2::Normalize(*this);
}

void Vector2::Set(float newX, float newY)
{
	x = newX;
	y = newY;
}

Vector2& Vector2::operator += (const Vector2& a)
{
	x += a.x;
	y += a.y;

	return (*this);
}

Vector2& Vector2::operator -= (const Vector2& a)
{
	return operator+=(-a);
}

Vector2& Vector2::operator *= (const Vector2& a)
{
	x *= a.x;
	y *= a.y;

	return (*this);
}

Vector2& Vector2::operator *= (float f)
{
	x *= f;
	y *= f;

	return (*this);
}

Vector2& Vector2::operator /= (float f)
{
	return operator*=(1.0f / f);
}