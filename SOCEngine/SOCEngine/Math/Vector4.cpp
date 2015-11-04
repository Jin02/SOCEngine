#include "Vector4.h"
#include "MathCommon.h"

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

	Vector4 Vector4::operator + () const
	{
		return (*this);
	}

	Vector4 Vector4::operator - () const
	{
		return Vector4(-x, -y, -z, -w);
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

	bool Vector4::operator == (const Vector4& b) const
	{
		bool x = abs(this->x - b.x) < FLT_EPSILON;
		bool y = abs(this->y - b.y) < FLT_EPSILON;
		bool z = abs(this->z - b.z) < FLT_EPSILON;
		bool w = abs(this->w - b.w) < FLT_EPSILON;

		return x & y & z & w;
	}

	bool Vector4::operator != (const Vector4& b) const
	{
		return !Vector4::operator==(b);
	}

	Vector4 Vector4::operator * (float d) const
	{
		return Vector4(x * d, y * d, z * d, w * d);
	}

	Vector4& Vector4::operator *= (float f)
	{
		x *= f; y *= f; z *= f; w *= f;
		return (*this);
	}

	Vector4& Vector4::operator -= (const Vector4& a)
	{
		x -= a.x; y -= a.y; z -= a.z;w -= a.z;
		return (*this);
	}

	Vector4 Vector4::operator / (float d) const
	{
		return Vector4(x/d, y/d, z/d, w/d);
	}

	float Vector4::operator[](unsigned int idx) const
	{
		if( idx == 0 )		return x;
		else if( idx == 1 ) return y;
		else if( idx == 2 ) return z;
		else if( idx == 3 ) return w;

		return 0.0f;
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

	float Vector4::Dot(const Vector4& a, const Vector4& b)
	{
		return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
	}

	Vector4 Vector4::Lerp(const Vector4& from, const Vector4& to, float t)
	{ 
		Vector4 a = to - from;
		return (a * t) + from;
	}

	Vector4 Vector4::Min(const Vector4& a, const Vector4& b)
	{
		float x = a.x > b.x ? b.x : a.x;
		float y = a.y > b.y ? b.y : a.y;
		float z = a.z > b.z ? b.z : a.z;
		float w = a.w > b.w ? b.w : a.w;

		return Vector4(x, y, z, w);
	}

	Vector4 Vector4::Max(const Vector4& a, const Vector4& b)
	{
		float x = a.x > b.x ? a.x : b.x;
		float y = a.y > b.y ? a.y : b.y;
		float z = a.z > b.z ? a.z : b.z;
		float w = a.w > b.w ? a.w : b.w;

		return Vector4(x, y, z, w);
	}

	float Vector4::Legnth(const Vector4& a)
	{
		return sqrtf((a.x * a.x) + (a.y + a.y) + (a.z * a.z) + (a.w * a.w));
	}

	Vector4 Vector4::Normalize(const Vector4& value)
	{
		float v = Vector4::Legnth(value);
		return value / v;
	}

	float Vector4::SqrLegnth(const Vector4& a)
	{
		return (a.x * a.x) + (a.y + a.y) + (a.z * a.z) + (a.w * a.w);
	}

	Vector4 Vector4::Normalize() const
	{
		return Vector4::Normalize(*this);
	}

	void Vector4::Set(float newX, float newY, float newZ, float newW)
	{
		x = newX;
		y = newY;
		z = newZ;
		w = newW;
	}

	Vector4& Vector4::operator += (const Vector4& a)
	{
		x += a.x; y += a.y; z += a.z;w += a.z;
		return (*this);
	}

	Vector4& Vector4::operator /= (float f)
	{
		x /= f; y /= f; z /= f; w /= f;
		return (*this);
	}
}