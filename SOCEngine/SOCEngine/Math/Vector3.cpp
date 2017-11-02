#include "Vector3.h"
#include "Matrix.h"

#include "Common.h"
#include <assert.h>
#include <math.h>
#include <float.h>

using namespace Math;

Vector3::Vector3(float _x, float _y, float _z)
	: x(_x), y(_y), z(_z)
{
}

const Vector3 Vector3::operator + () const
{
	return (*this);
}

const Vector3 Vector3::operator - () const
{
	return Vector3(-x, -y, -z);
}

const Vector3 Vector3::operator - (const Vector3& a) const
{
	return Vector3(x - a.x, y - a.y, z - a.z);
}

const Vector3 Vector3::operator + (const Vector3& a) const
{
	return Vector3(a.x + x, a.y + y, a.z + z);
}

bool Vector3::operator == (const Vector3& b) const
{
	return (fabsf(x - b.x) < FLT_EPSILON) & (fabsf(y - b.y) < FLT_EPSILON) & (fabsf(z - b.z) < FLT_EPSILON);
}

bool Vector3::operator != (const Vector3& b) const
{
	return !Vector3::operator==(b);
}

const Vector3 Vector3::operator * (float d) const
{
	return Vector3(x * d, y * d, z * d);
}

const Vector3 Vector3::operator * (const Vector3& a) const
{
	return Vector3(a.x * x, a.y * y, a.z * z);
}

const Vector3 Vector3::operator / (float d) const
{
	return Vector3(x / d, y / d, z / d);
}

float Vector3::operator[](unsigned int idx) const
{
	return value[idx];
}

Vector3& Vector3::operator += (const Vector3& a)
{
	x += a.x;
	y += a.y;
	z += a.z;

	return (*this);
}

Vector3& Vector3::operator -= (const Vector3& a)
{
	x -= a.x;
	y -= a.y;
	z -= a.z;

	return (*this);
}

Vector3& Vector3::operator *= (const Vector3& a)
{
	x *= a.x;
	y *= a.y;
	z *= a.z;

	return (*this);
}

Vector3& Vector3::operator *= (float f)
{
	x *= f; y *= f; z *= f;
	return (*this);
}

Vector3& Vector3::operator /= (float f)
{
	x /= f; y /= f; z /= f;
	return (*this);
}

const Vector3 Vector3::Forward()
{
	return Vector3(0, 0, 1);
}

const Vector3 Vector3::Right()
{
	return Vector3(1, 0, 0);
}

const Vector3 Vector3::One()
{
	return Vector3(1, 1, 1);
}

const Vector3 Vector3::Up()
{
	return Vector3(0, 1, 0);
}

const Vector3 Vector3::Zero()
{
	return Vector3(0, 0, 0);
}

float Vector3::AnglebyDirect(const Vector3& from, const Vector3& to, bool radian)
{
	float dot = Vector3::Dot(from, to);
	return radian ? acosf(dot) : RAD_TO_DEG(acosf(dot));
}

float Vector3::AnglebyPoint(const Vector3& from, const Vector3& to, bool radian)
{
	float sx = (from.x - to.x) * (from.x - to.x);
	float sy = (from.y - to.y) * (from.y - to.y);
	float z = (from.z - to.z);
	float distanceXtoY = sqrtf(sx + sy);

	return radian ? atan2f(z, distanceXtoY) : RAD_TO_DEG(atan2f(z, distanceXtoY));
}

const Vector3 Vector3::Cross(const Vector3& a, const Vector3& b)
{
	return Vector3(a.y * b.z - b.y * a.z, a.z * b.x - b.z * a.x, a.x * b.y - b.x * a.y);
}

float Vector3::Distance(const Vector3& a, const Vector3& b)
{
	float sx = (b.x - a.x) * (b.x - a.x);
	float sy = (b.y - a.y) * (b.y - a.y);
	float sz = (b.z - a.z) * (b.z - a.z);

	return sqrtf(sx + sy + sz);
}

float Vector3::Dot(const Vector3& a, const Vector3& b)
{
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

const Vector3 Vector3::Lerp(const Vector3& from, const Vector3& to, float t)
{
	Vector3 a = to - from;
	return (a * t) + from;
}

const Vector3 Vector3::Min(const Vector3& a, const Vector3& b)
{
	float x = a.x > b.x ? b.x : a.x;
	float y = a.y > b.y ? b.y : a.y;
	float z = a.z > b.z ? b.z : a.z;

	return Vector3(x, y, z);
}

const Vector3 Vector3::Max(const Vector3& a, const Vector3& b)
{
	float x = a.x > b.x ? a.x : b.x;
	float y = a.y > b.y ? a.y : b.y;
	float z = a.z > b.z ? a.z : b.z;

	return Vector3(x, y, z);
}

float Vector3::Length(const Vector3& a)
{
	return sqrtf((a.x * a.x) + (a.y * a.y) + (a.z * a.z));
}

const Vector3 Vector3::Normalize(const Vector3& value)
{
	float v = Vector3::Length(value);
	assert(v > 0.0f);
	return value / v;
}

const Vector3 Vector3::Project(const Vector3& vector, const Vector3& onNormal)
{
	float dot = Vector3::Dot(vector, onNormal);
	float t = dot / Vector3::SqrLegnth(onNormal);
	return vector * t;
}

const Vector3 Vector3::Reflect(const Vector3& inDirection, const Vector3& inNormal)
{
	float dot = Vector3::Dot(inDirection, inNormal);

	float x = -2.0f * dot * inNormal.x;
	float y = -2.0f * dot * inNormal.y;
	float z = -2.0f * dot * inNormal.z;

	return inDirection - Vector3(x, y, z);
}

float Vector3::SqrLegnth(const Vector3& a)
{
	return (a.x * a.x) + (a.y + a.y) + (a.z * a.z);
}

const Vector3 Vector3::Normalized() const
{
	return Vector3::Normalize(*this);
}

float Vector3::Length() const
{
	return Vector3::Length((*this));
}

float Vector3::Dot(const Vector3& v) const
{
	return Vector3::Dot((*this), v);
}

void Vector3::Set(float newX, float newY, float newZ)
{
	x = newX;
	y = newY;
	z = newZ;
}

Vector3 Vector3::TransformCoord(const Vector3& v, const Matrix& mat)
{
	Vector3 src = v;
	float w = mat._m[0][3] * src.x + mat._m[1][3] * src.y + mat._m[2][3] * src.z + mat._m[3][3];

	if (w == 0.0f)
		return Vector3(0.0f, 0.0f, 0.0f);

	return Vector3(
	(mat._m[0][0] * src.x + mat._m[1][0] * src.y + mat._m[2][0] * src.z + mat._m[3][0]) / w,
	(mat._m[0][1] * src.x + mat._m[1][1] * src.y + mat._m[2][1] * src.z + mat._m[3][1]) / w,
	(mat._m[0][2] * src.x + mat._m[1][2] * src.y + mat._m[2][2] * src.z + mat._m[3][2]) / w );
}

Vector3 Vector3::TransformNormal(const Vector3& v, const Matrix& mat)
{
	Vector3 src = v;

	return Vector3(
	(mat._m[0][0] * src.x + mat._m[1][0] * src.y + mat._m[2][0] * src.z),
	(mat._m[0][1] * src.x + mat._m[1][1] * src.y + mat._m[2][1] * src.z),
	(mat._m[0][2] * src.x + mat._m[1][2] * src.y + mat._m[2][2] * src.z) );
}

//http://www.gamedev.net/topic/643623-how-do-i-get-the-euler-angle-from-a-matrix/
Vector3 Vector3::FromRotationMatrix(const Matrix& matrix)
{
	Vector3 dotx = Vector3(matrix._11, matrix._12, matrix._13);
	Vector3 doty = Vector3(matrix._21, matrix._22, matrix._23);
	Vector3 dotz = Vector3(matrix._31, matrix._32, matrix._33);

	float x = Vector3::Dot(dotx, dotx);

	auto Approximately = [](float a, float b) -> bool
	{
		return fabsf(a - b) < FLT_EPSILON;
	};

	if (Approximately(x, 1.0f) == false)
	{
		float invx = 1.0f / sqrtf(x);
		dotx = dotx*invx;
	}

	float y = Vector3::Dot(doty, doty);
	if (Approximately(y, 1.0f) == false)
	{
		float invy = 1.0f / sqrtf(y);
		doty = doty*invy;
	}

	float z = Vector3::Dot(dotz, dotz);
	if (Approximately(z, 1.0f) == false)
	{
		float invz = 1.0f / sqrtf(z);
		dotz = dotz*invz;
	}

	float thetaX = 0;
	float thetaY = 0;
	float thetaZ = 0;
	if (dotz.y < 1.0f)
	{
		if (dotz.y > -1.0f)
		{
			thetaX = asinf(-dotz.y);
			thetaY = atan2f(dotz.x, dotz.z);
			thetaZ = atan2f(dotx.y, doty.y);
		}
		else
		{
			thetaX = MATH_PI*0.5f;
			thetaY = -atan2f(-doty.x, dotx.x);
			thetaZ = 0;
		}
	}
	else
	{
		thetaX = -MATH_PI * 0.5f;
		thetaY = atan2f(-doty.x, dotx.x);
		thetaZ = 0;
	}

	return Vector3(thetaX, thetaY, thetaZ);
}