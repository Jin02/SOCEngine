#include "Vector2.h"

namespace Math
{
	Vector2::Vector2(void) :
		x(0.0f), y(0.0f)
	{
	}

	Vector2::Vector2(float x, float y)
	{
		this->x = x;
		this->y = y;
	}

	Vector2::~Vector2(void)
	{
	}

	Vector2 Vector2::operator - (const Vector2 &a) const
	{
		return Vector2(x - a.x, y - a.y);
	}

	Vector2 Vector2::operator + (const Vector2 &a) const
	{
		return Vector2( a.x + x, a.y + y );
	}

	bool Vector2::operator == (const Vector2 rhs) const
	{
		const float kEpsilon = 1e-005f;
		bool x = abs(this->x - rhs.x) < kEpsilon;
		bool y = abs(this->y - rhs.y) < kEpsilon;

		return x & y;
	}

	bool Vector2::operator != (const Vector2 &rhs) const
	{
		return !Vector2::operator==(rhs);
	}

	Vector2 Vector2::operator * (float d) const
	{
		return Vector2(x * d, y * d);
	}

	Vector2 Vector2::operator * (const Vector2 &a) const
	{
		return Vector2(a.x * x, a.y * y);
	}

	Vector2 Vector2::operator / (float d) const
	{
		return Vector2(x/d, y/d);
	}

	Vector2 Vector2::Zero()
	{
		return Vector2(0, 0);
	}

	Vector2 Vector2::One()
	{
		return Vector2(1, 1);
	}

	float Vector2::Angle(const Vector2 &from, const Vector2 &to, bool radian)
	{
		float x = to.x - from.x;
		float y = to.y - from.y;

		return radian ? atan2f(y, x) : atan2f(y, x) * Common::Rad2Deg();
	}

	float Vector2::Distance(const Vector2 &a, const Vector2 &b)
	{
		float sx = (b.x - a.x) * (b.x - a.x);
		float sy = (b.y - a.y) * (b.y - a.y);

		return sqrtf(sx+sy);
	}

	float Vector2::Dot(const Vector2 &lhs, const Vector2 &rhs)
	{
		return (lhs.x * rhs.x) + (lhs.y * rhs.y);
	}

	Vector2 Vector2::Lerp(const Vector2 &from, const Vector2 &to, float t)
	{
		Vector2 a = to - from;
		return a * t;
	}

	Vector2 Vector2::Min(const Vector2 &lhs, const Vector2 &rhs)
	{
		float x = lhs.x > rhs.x ? rhs.x : lhs.x;
		float y = lhs.y > rhs.y ? rhs.y : lhs.y;

		return Vector2(x, y);
	}

	Vector2 Vector2::Max(const Vector2 &lhs, const Vector2 &rhs)
	{
		float x = lhs.x > rhs.x ? lhs.x : rhs.x;
		float y = lhs.y > rhs.y ? lhs.y : rhs.y;

		return Vector2(x, y);
	}

	float Vector2::Legnth(const Vector2 &a)
	{
		return sqrtf((a.x * a.x) + (a.y + a.y));
	}

	Vector2 Vector2::Normalize(const Vector2 &value)
	{
		float v = Vector2::Legnth(value);
		return value / v;
	}

	Vector2 Vector2::Scale(const Vector2 &a, const Vector2 &b)
	{
		return Vector2(a.x * b.x, a.y * b.y);
	}

	float Vector2::SqrLegnth(const Vector2 &a)
	{
		return (a.x * a.x) + (a.y + a.y);
	}

	void Vector2::Normalize()
	{
		float v = Vector2::Legnth(*this);
		*this = *this / v;
	}

	void Vector2::Scale(const Vector2 &scale)
	{
		x *= scale.x;
		y *= scale.y;
	}

	void Vector2::Set(float newX, float newY)
	{
		x = newX;
		y = newY;
	}
}