#include "Vector3.h"
#include "MathCommon.h"
#include "Matrix.h"

namespace Math
{
	Vector3::Vector3() :
		x(0.0f), y(0.0f), z(0.0f)
	{
	}

	Vector3::Vector3(float x, float y)
		: z(0.0f)
	{
		this->x = x;
		this->y = y;
	}

	Vector3::Vector3(float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	Vector3::~Vector3(void)
	{
	}

	Vector3 Vector3::operator + () const
	{
		return (*this);
	}

	Vector3 Vector3::operator - () const
	{
		return Vector3(-x, -y, -z);
	}

	Vector3 Vector3::operator - (const Vector3& a) const
	{
		return Vector3(x-a.x, y-a.y, z-a.z);
	}

	Vector3 Vector3::operator + (const Vector3& a) const
	{
		return Vector3( a.x + x,
			a.y + y,
			a.z + z);
	}

	bool Vector3::operator == (const Vector3& b) const
	{
		bool x = abs(this->x - b.x) < FLT_EPSILON;
		bool y = abs(this->y - b.y) < FLT_EPSILON;
		bool z = abs(this->z - b.z) < FLT_EPSILON;

		return x & y & z;
	}

	bool Vector3::operator != (const Vector3& b) const
	{
		return !Vector3::operator==(b);
	}

	Vector3 Vector3::operator * (float d) const
	{
		return Vector3(x * d, y * d, z * d);
	}

	Vector3 Vector3::operator * (const Vector3& a) const
	{
		return Vector3(a.x * x, a.y * y, a.z * z);
	}

	Vector3 Vector3::operator / (float d) const
	{
		return Vector3(x/d, y/d, z/d);
	}

	float Vector3::operator[](unsigned int idx) const
	{
		if(idx == 0)			return x;
		else if(idx == 1)		return y;
		else if(idx == 2)		return z;

		return 0.0f;
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

	Vector3 Vector3::Forward()
	{
		return Vector3(0, 0, 1);
	}

	Vector3 Vector3::Right()
	{
		return Vector3(1, 0, 0);
	}

	Vector3 Vector3::One()
	{
		return Vector3(1, 1, 1);
	}

	Vector3 Vector3::Up()
	{
		return Vector3(0, 1, 0);
	}

	Vector3 Vector3::Zero()
	{
		return Vector3(0, 0, 0);
	}

	float Vector3::AnglebyDirect(const Vector3& from, const Vector3& to, bool radian)
	{
		float dot = Vector3::Dot(from, to);
		return radian ? acos(dot) : Common::Rad2Deg( acos(dot) );
	}

	float AnglebyPoint(const Vector3& from, const Vector3& to, bool radian = true)
	{
		float sx = (from.x - to.x) * (from.x - to.x);
		float sy = (from.y - to.y) * (from.y - to.y);
		float z  = (from.z - to.z);
		float distanceXtoY = sqrtf(sx+sy);

		return radian ? atan2f(z, distanceXtoY) : Common::Rad2Deg( atan2f(z, distanceXtoY));
	}

	Vector3 Vector3::Cross(const Vector3& a, const Vector3& b)
	{
		return Vector3( a.y * b.z - b.y * a.z, a.z * b.x - b.z * a.x, a.x * b.y - b.x * a.y );
	}

	float Vector3::Distance(const Vector3& a, const Vector3& b)
	{
		float sx = (b.x - a.x) * (b.x - a.x);
		float sy = (b.y - a.y) * (b.y - a.y);
		float sz = (b.z - a.z) * (b.z - a.z);

		return sqrtf(sx+sy+sz);
	}

	float Vector3::Dot(const Vector3& a, const Vector3& b)
	{
		return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
	}

	Vector3 Vector3::Lerp(const Vector3& from, const Vector3& to, float t)
	{
		Vector3 a = to - from;
		return (a * t) + from;
	}

	Vector3 Vector3::Min(const Vector3& a, const Vector3& b)
	{
		float x = a.x > b.x ? b.x : a.x;
		float y = a.y > b.y ? b.y : a.y;
		float z = a.z > b.z ? b.z : a.z;

		return Vector3(x, y, z);
	}

	Vector3 Vector3::Max(const Vector3& a, const Vector3& b)
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

	Vector3 Vector3::Normalize(const Vector3& value)
	{
		float v = Vector3::Length(value);
		return value / v;
	}

	Vector3 Vector3::Project(const Vector3& vector, const Vector3& onNormal)
	{
		float dot = Vector3::Dot(vector, onNormal);
		float t = dot / Vector3::SqrLegnth(onNormal);
		return vector * t;
	}

	Vector3 Vector3::Reflect(const Vector3& inDirection, const Vector3& inNormal)
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

	Vector3 Vector3::Normalized() const
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

	void Vector3::TransformCoord(Vector3& out, const Vector3& v, const Matrix& mat)
	{
		Vector3 src = v;
		float w = mat._m[0][3] * src.x + mat._m[1][3] * src.y + mat._m[2][3] * src.z + mat._m[3][3];

		if(w == 0.0f)
		{
			out.x = out.y = out.z = 0.0f;
			return;
		}

		out.x = (mat._m[0][0] * src.x + mat._m[1][0] * src.y + mat._m[2][0] * src.z + mat._m[3][0]) / w;
		out.y = (mat._m[0][1] * src.x + mat._m[1][1] * src.y + mat._m[2][1] * src.z + mat._m[3][1]) / w;
		out.z = (mat._m[0][2] * src.x + mat._m[1][2] * src.y + mat._m[2][2] * src.z + mat._m[3][2]) / w;
	}

	void Vector3::TransformNormal(Vector3& out, const Vector3& v, const Matrix& mat)
	{
		Vector3 src = v;

		out.x = (mat._m[0][0] * src.x + mat._m[1][0] * src.y + mat._m[2][0] * src.z);
		out.y = (mat._m[0][1] * src.x + mat._m[1][1] * src.y + mat._m[2][1] * src.z);
		out.z = (mat._m[0][2] * src.x + mat._m[1][2] * src.y + mat._m[2][2] * src.z);
	}

	//http://www.gamedev.net/topic/643623-how-do-i-get-the-euler-angle-from-a-matrix/
	void Vector3::FromRotationMatrix(Vector3& out, const Matrix& matrix)
	{
		Vector3 dotx = Vector3(matrix._11, matrix._12, matrix._13);
		Vector3 doty = Vector3(matrix._21, matrix._22, matrix._23);
		Vector3 dotz = Vector3(matrix._31, matrix._32, matrix._33);

		float x = Vector3::Dot(dotx, dotx);

		auto Approximately =[](float a, float b) -> bool
		{
			return abs(a - b) < FLT_EPSILON;
		};

		if(Approximately(x, 1.0f)==false)
		{
			float invx = 1.0f/sqrt(x);
			dotx = dotx*invx;
		}

		float y = Vector3::Dot(doty,doty);
		if(Approximately(y, 1.0f)==false)
		{
			float invy = 1.0f/sqrt(y);
			doty = doty*invy;
		}

		float z = Vector3::Dot(dotz,dotz);
		if(Approximately(z,1.0f)==false)
		{
			float invz = 1.0f/sqrt(z);
			dotz = dotz*invz;
		}

		float thetaX=0;
		float thetaY=0;
		float thetaZ=0;
		if(dotz.y < 1.0f)
		{
			if(dotz.y > -1.0f)
			{
				thetaX = asin(-dotz.y);
				thetaY = atan2(dotz.x, dotz.z);
				thetaZ = atan2(dotx.y, doty.y);
			}
			else
			{
				thetaX = MATH_PI*0.5f;
				thetaY = -atan2(-doty.x, dotx.x);
				thetaZ = 0;
			}
		}
		else
		{
			thetaX = -MATH_PI * 0.5f;
			thetaY = atan2(-doty.x, dotx.x);
			thetaZ =0;
		}   

		out.x = thetaX;
		out.y = thetaY;
		out.z = thetaZ;
	}
}