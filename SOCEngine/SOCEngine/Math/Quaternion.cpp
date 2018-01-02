#include "Quaternion.h"
#include "Vector3.h"
#include "Matrix.h"

#include <math.h>
#include <float.h>

using namespace Math;

Quaternion::Quaternion(float _x, float _y, float _z, float _w)
	: x(_x), y(_y), z(_z), w(_w)
{
}

const Quaternion Quaternion::operator + () const
{
	return (*this);
}
const Quaternion Quaternion::operator - () const
{
	return Quaternion(-x, -y, -z, -w);
}

const Quaternion Quaternion::operator + (const Quaternion& a) const
{
	return Quaternion(x + a.x, y + a.y, z + a.z, w + a.w).Normalized();
}
const Quaternion Quaternion::operator - (const Quaternion& a) const
{
	return Quaternion(x - a.x, y - a.y, z - a.z, w - a.w).Normalized();
}
const Quaternion Quaternion::operator * (const Quaternion& a) const
{
	return Quaternion(	(w * a.x) + (x * a.w) + (y * a.z) - (z * a.y),
						(w * a.y) + (y * a.w) + (z * a.x) - (x * a.z),
						(w * a.z) + (z * a.w) + (x * a.y) - (y * a.x),
						(w * a.w) - (x * a.x) - (y * a.y) - (z * a.z)	);
}

const Quaternion Quaternion::operator * (float f) const
{
	return Quaternion(x * f, y * f, z * f, w * f);
}

const Quaternion Quaternion::operator / (float f) const
{
	return Quaternion(x / f, y / f, z / f, w / f);
}

bool Quaternion::operator == (const Quaternion& a) const
{
	Quaternion q = (*this) - a;
	return (fabsf(q.x) < FLT_EPSILON) & (fabsf(q.y) < FLT_EPSILON) & (fabsf(q.z) < FLT_EPSILON) & (fabsf(q.w) < FLT_EPSILON);
}
bool Quaternion::operator != (const Quaternion& a) const
{
	return !operator==(a);
}

Quaternion& Quaternion::operator += (const Quaternion& a)
{
	x += a.x;
	y += a.y;
	z += a.z;
	w += a.w;

	(*this) = Normalize(*this);
	return (*this);
}

Quaternion& Quaternion::operator -= (const Quaternion& a)
{
	x -= a.x;
	y -= a.y;
	z -= a.z;
	w -= a.w;

	(*this) = Normalize(*this);
	return (*this);
}

Quaternion& Quaternion::operator *= (const Quaternion& q)
{
	x = (w * q.x) + (x * q.w) + (y * q.z) - (z * q.y);
	y = (w * q.y) + (y * q.w) + (z * q.x) - (x * q.z);
	z = (w * q.z) + (z * q.w) + (x * q.y) - (y * q.x);
	w = (w * q.w) - (x * q.x) - (y * q.y) - (z * q.z);

	(*this) = Normalize(*this);
	return (*this);
}

Quaternion Quaternion::Inverse(const Quaternion& in)
{
	float sq = (in.x * in.x) + (in.y + in.y) + (in.z * in.z) + (in.w * in.w);
	float inv_norm = 1.0f / sq;

	return Quaternion(-in.x * inv_norm, -in.y * inv_norm, -in.z * inv_norm, -in.w * inv_norm);
}

Quaternion Quaternion::Normalize(const Quaternion& in)
{
	float sq = (in.x * in.x) + (in.y * in.y) + (in.z * in.z) + (in.w * in.w);
	float dst = 1.0f / sqrtf(sq);

	return Quaternion(in.x * dst, in.y * dst, in.z * dst, in.w * dst);
}

float Quaternion::Dot(const Quaternion& a, const Quaternion& b)
{
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
}

Quaternion Quaternion::Slerp(const Quaternion& from, const Quaternion& to, float t)
{
	float fromWeight, toWeight;

	float diff = (from.x * to.x) + (from.y * to.y) + (from.z * to.z) + (from.w * to.w);

	if ((1.0f - fabsf(diff)) > FLT_EPSILON)
	{		
		float theta = acosf(fabsf(diff));
		float oneOverSinTheta = 1.0f / sinf(theta);

		fromWeight = sinf(theta * (1.0f - t)) * oneOverSinTheta;
		toWeight = sinf(theta * t) * oneOverSinTheta;

		if (diff < 0.0f)
			fromWeight *= -1.0f;
	}
	else
	{
		fromWeight = 1.0f - t;
		toWeight = t;
	}

	return Quaternion(	(from.x * fromWeight) + (to.x  * toWeight),
						(from.y * fromWeight) + (to.y  * toWeight),
						(from.z * fromWeight) + (to.z  * toWeight),
						(from.w * fromWeight) + (to.w  * toWeight) ).Normalized();
}

Quaternion Quaternion::Lerp(const Quaternion& from, const Quaternion& to, float t)
{
	float diff = (from.x * to.x) + (from.y * to.y) + (from.z * to.z) + (from.w * to.w);

	float fromWeight = 1.0f - t;
	float toWeight = t;

	return Quaternion(	(from.x * fromWeight) + (to.x * toWeight),
						(from.y * fromWeight) + (to.y * toWeight),
						(from.z * fromWeight) + (to.z * toWeight),
						(from.w * fromWeight) + (to.w * toWeight)	);
}

Quaternion Quaternion::FromEuler(const Vector3& v)
{
	return FromYawPitchRoll(v.y, v.x, v.z);
}

void Quaternion::Set(float x, float y, float z, float w)
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

Quaternion Quaternion::FromRotationMatrix(const Matrix& m)
{
	Quaternion out;

	float s = 0.0f;
	float trace = m._m[0][0] + m._m[1][1] + m._m[2][2] + 1.0f;

	if (trace > 1.0f)
	{
		s = 2.0f * sqrtf(trace);
		out.x = (m._m[1][2] - m._m[2][1]) / s;
		out.y = (m._m[2][0] - m._m[0][2]) / s;
		out.z = (m._m[0][1] - m._m[1][0]) / s;
		out.w = 0.25f * s;
	}
	else
	{
		int i, maxi = 0;

		for (i = 1; i < 3; i++)
		{
			if (m._m[i][i] > m._m[maxi][maxi])
				maxi = i;
		}

		switch (maxi)
		{
		case 0:
			s = 2.0f * sqrtf(1.0f + m._m[0][0] - m._m[1][1] - m._m[2][2]);
			out.x = 0.25f * s;
			out.y = (m._m[0][1] + m._m[1][0]) / s;
			out.z = (m._m[0][2] + m._m[2][0]) / s;
			out.w = (m._m[1][2] - m._m[2][1]) / s;
			break;

		case 1:
			s = 2.0f * sqrtf(1.0f + m._m[1][1] - m._m[0][0] - m._m[2][2]);
			out.x = (m._m[0][1] + m._m[1][0]) / s;
			out.y = 0.25f * s;
			out.z = (m._m[1][2] + m._m[2][1]) / s;
			out.w = (m._m[2][0] - m._m[0][2]) / s;
			break;

		case 2:
			s = 2.0f * sqrtf(1.0f + m._m[2][2] - m._m[0][0] - m._m[1][1]);
			out.x = (m._m[0][2] + m._m[2][0]) / s;
			out.y = (m._m[1][2] + m._m[2][1]) / s;
			out.z = 0.25f * s;
			out.w = (m._m[0][1] - m._m[1][0]) / s;
			break;
		}
	}

	return out;
}

Quaternion Quaternion::FromYawPitchRoll(float yaw, float pitch, float roll)
{
	float syaw		= sinf(yaw / 2.0f);
	float cyaw		= cosf(yaw / 2.0f);
	float spitch	= sinf(pitch / 2.0f);
	float cpitch	= cosf(pitch / 2.0f);
	float sroll		= sinf(roll / 2.0f);
	float croll		= cosf(roll / 2.0f);

	return Quaternion(	syaw * cpitch * sroll + cyaw * spitch * croll,
						syaw * cpitch * croll - cyaw * spitch * sroll,
						cyaw * cpitch * sroll - syaw * spitch * croll,
						cyaw * cpitch * croll + syaw * spitch * sroll	);
}

Quaternion Quaternion::FromAxis(const Vector3& axis, float angle)
{
	Vector3 temp = Vector3::Normalize(axis);

	return Quaternion(	sinf(angle / 2.0f) * temp.x,
						sinf(angle / 2.0f) * temp.y,
						sinf(angle / 2.0f) * temp.z,
						cosf(angle / 2.0f)	);
}

const Quaternion Quaternion::Normalized()
{
	return Quaternion::Normalize(*this);
}

const Quaternion Quaternion::Inversed()
{
	return Inverse(*this);
}

const Vector3 Math::Quaternion::GetForward() const
{
	return Vector3(	2.0f * (x * z - y * w),
					2.0f * (y *z + x * w),
					1.0f - 2.0f * (x * x + y * y)	);
}

const Vector3 Math::Quaternion::GetRight() const
{
	return Vector3(	1.0f - 2.0f * (y * y + z * z),
					2.0f * (x * y - z * w),
					2.0f * (x * z + y * w)	);
}

const Vector3 Math::Quaternion::GetUp() const
{
	return Vector3(	2.0f * (x *y + z * w),
					1.0f - 2.0f * (x * x + z * z),
					2.0f * (y *z - x * w)	);
}
