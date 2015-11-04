#include "Quaternion.h"
#include "Vector3.h"
#include "Matrix.h"

namespace Math
{
	Quaternion::Quaternion() : x(0.0f), y(0.0f), z(0.0f), w(0.0f)
	{
	}

	Quaternion::Quaternion(float _x, float _y, float _z, float _w)
		:x(_x), y(_y), z(_z), w(_w)
	{
	}

	Quaternion::~Quaternion()
	{
	}

	Quaternion Quaternion::operator + () const
	{
		return (*this);
	}
	Quaternion Quaternion::operator - () const
	{
		return Quaternion(-x, -y, -z, -w);
	}

	Quaternion Quaternion::operator + (const Quaternion& a) const
	{
		Quaternion q(x+a.x, y+a.y, z+a.z, w+a.w);
		Normalize(q, q);

		return q;
	}
	Quaternion Quaternion::operator - (const Quaternion& a) const
	{
		Quaternion q(x-a.x, y-a.y, z-a.z, w-a.w);
		Normalize(q, q);

		return q;
	}
	Quaternion Quaternion::operator * (const Quaternion& a) const
	{
		Quaternion q;
		q.x = (w * a.x) + (x * a.w) + (y * a.z) - (z * a.y);
		q.y = (w * a.y) + (y * a.w) + (z * a.x) - (x * a.z);
		q.z = (w * a.z) + (z * a.w) + (x * a.y) - (y * a.x);
		q.w = (w * a.w) - (x * a.x) - (y * a.y) - (z * a.z);

		return q;
	}
	Quaternion Quaternion::operator * (float f) const
	{
		return Quaternion(x*f, y*f, z*f, w*f);
	}
	Quaternion Quaternion::operator / (float f) const
	{
		return Quaternion(x/f, y/f, z/f, w/f);
	}
	bool Quaternion::operator == (const Quaternion& a) const
	{
		Quaternion q = (*this) - a;

		if( abs(q.x) < FLT_EPSILON &&
			abs(q.y) < FLT_EPSILON &&
			abs(q.z) < FLT_EPSILON && 
			abs(q.w) < FLT_EPSILON)
			return true;

		return false;
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

		Normalize((*this), (*this));

		return (*this);
	}

	Quaternion& Quaternion::operator -= (const Quaternion& a)
	{
		x -= a.x;
		y -= a.y;
		z -= a.z;
		w -= a.w;

		Normalize((*this), (*this));

		return (*this);
	}

	Quaternion& Quaternion::operator *= (const Quaternion& q)
	{
		x = (w * q.x) + (x * q.w) + (y * q.z) - (z * q.y);
		y = (w * q.y) + (y * q.w) + (z * q.x) - (x * q.z);
		z = (w * q.z) + (z * q.w) + (x * q.y) - (y * q.x);
		w = (w * q.w) - (x * q.x) - (y * q.y) - (z * q.z);

		Normalize((*this), (*this));
		return (*this);
	}

	void Quaternion::Inverse(Quaternion& out, const Quaternion& in)
	{
		float sq = (in.x * in.x) + (in.y + in.y) + (in.z * in.z) + (in.w * in.w);
		float inv_norm = 1.0f / sq;

		out.x = -in.x * inv_norm;
		out.y = -in.y * inv_norm;
		out.z = -in.z * inv_norm;
		out.w = -in.w * inv_norm;
	}

	void Quaternion::Normalize(Quaternion& out, const Quaternion& in)
	{
		float sq = (in.x * in.x) + (in.y * in.y) + (in.z * in.z) + (in.w * in.w);
		float dst = 1.0f / sqrt(sq);

		out.x = in.x * dst;
		out.y = in.y * dst;
		out.z = in.z * dst;
		out.w = in.w * dst;
	}

	void Quaternion::Dot(float& out, const Quaternion& a, const Quaternion& b)
	{
		out = (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
	}

	void Quaternion::Slerp(Quaternion& out, const Quaternion& from, const Quaternion& to, float t)
	{
		float fromWeight, toWeight;

		float diff = (from.x * to.x) + (from.y * to.y) + (from.z * to.z) + (from.w * to.w);

		if( (1.0f - fabs(diff)) > FLT_EPSILON )
		{
			float theta = acos( fabs(diff) );
			float oneOverSinTheta = 1.0f / sin(theta);

			fromWeight = sin(theta * (1.0f - t)) * oneOverSinTheta;
			toWeight = sin(theta * t) * oneOverSinTheta;

			if(diff < 0.0f)
				fromWeight *= -1.0f;
		}
		else
		{
			fromWeight = 1.0f - t;
			toWeight = t;
		}

		out.x = (from.x * fromWeight) + (to.x  * toWeight);
		out.y = (from.y * fromWeight) + (to.y  * toWeight);
		out.z = (from.z * fromWeight) + (to.z  * toWeight);
		out.w = (from.w * fromWeight) + (to.w  * toWeight);

		Normalize(out, out);
	}

	void Quaternion::Lerp(Quaternion& out, const Quaternion& from, const Quaternion& to, float t)
	{
		float diff = (from.x * to.x) + (from.y * to.y) + (from.z * to.z) + (from.w * to.w);

		float fromWeight = 1.0f - t;
		float toWeight = t;

		out.x = (from.x * fromWeight) + (to.x * toWeight);
		out.y = (from.y * fromWeight) + (to.y * toWeight);
		out.z = (from.z * fromWeight) + (to.z * toWeight);
		out.w = (from.w * fromWeight) + (to.w * toWeight);
	}

	void Quaternion::ToEuler(Vector3& out, const Quaternion& q)
	{
		const float Epsilon = 0.0009765625f; 
		const float Threshold = 0.5f - Epsilon; 

		float yaw; 
		float pitch; 
		float roll; 

		float XY = q.x * q.y; 
		float ZW = q.z * q.w; 

		float TEST = XY + ZW; 

		if (TEST < -Threshold || TEST > Threshold)
		{ 
			int sign = TEST > 0 ? 1 : TEST == 0 ? 0 : -1;
			yaw  = sign * 2 * (float)atan2(q.x, q.w); 
			pitch = sign * MATH_PI / 2.0f; 
			roll = 0; 
		}
		else
		{ 
			float XX = q.x * q.x; 
			float XZ = q.x * q.z; 
			float XW = q.x * q.w; 

			float YY = q.y * q.y; 
			float YW = q.y * q.w; 
			float YZ = q.y * q.z; 

			float ZZ = q.z * q.z; 

			yaw = (float)atan2(2 * YW - 2 * XZ, 1 - 2 * YY - 2 * ZZ); 

			pitch = (float)atan2(2 * XW - 2 * YZ, 1 - 2 * XX - 2 * ZZ); 

			roll = (float)asin(2 * TEST); 

		} 

		out.x = pitch;
		out.y = yaw;
		out.z = roll;
	}

	void Quaternion::FromEuler(Quaternion& out, const Vector3& v)
	{
		float cosRoll	= cos(v.z / 2.0f);
		float cosPitch	= cos(v.x / 2.0f);
		float cosYaw	= cos(v.y / 2.0f);

		float sinRoll	= sin(v.z / 2.0f);
		float sinPitch	= sin(v.x / 2.0f);
		float sinYaw	= sin(v.y / 2.0f);

		out.w = (cosPitch * cosYaw * cosRoll) + (sinPitch * sinRoll * sinYaw);
		out.x = -( (sinPitch * cosYaw * cosRoll) - (cosPitch * sinYaw * sinRoll) );
		out.y = -( (cosPitch * sinYaw * cosRoll) + (sinPitch * cosYaw * sinRoll) );
		out.z = -( (cosPitch * cosYaw * sinRoll) - (sinPitch * sinYaw * cosRoll) );
	}

	void Quaternion::Set(float x,  float y, float z, float w)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}

	void Quaternion::RotationMatrix(Quaternion& out, const Matrix& m)
	{
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
	}

	void Quaternion::RotationYawPitchRoll(Quaternion& out, float yaw, float pitch, float roll)
	{
		float syaw, cyaw, spitch, cpitch, sroll, croll;      
		syaw = sinf(yaw / 2.0f);
		cyaw = cosf(yaw / 2.0f);
		spitch = sinf(pitch / 2.0f);
		cpitch = cosf(pitch / 2.0f);
		sroll = sinf(roll / 2.0f);
		croll = cosf(roll / 2.0f);

		out.x = syaw * cpitch * sroll + cyaw * spitch * croll;
		out.y = syaw * cpitch * croll - cyaw * spitch * sroll;
		out.z = cyaw * cpitch * sroll - syaw * spitch * croll;
		out.w = cyaw * cpitch * croll + syaw * spitch * sroll;
	}

	void Quaternion::RotationAxis(Quaternion& out, const Vector3& axis, float angle)
	{
		Vector3 temp = Vector3::Normalize(axis);

		out.x = sinf(angle / 2.0f) * temp.x;
		out.y = sinf(angle / 2.0f) * temp.y;
		out.z = sinf(angle / 2.0f) * temp.z;
		out.w = cosf(angle / 2.0f);
	}

	Quaternion Quaternion::Normalize()
	{
		Quaternion q;
		Quaternion::Normalize(q, *this);
		return q;
	}
	Quaternion Quaternion::Inverse()
	{
		Quaternion q;
		Inverse(q, *this);
		return q;
	}
	Vector3 Quaternion::ToEuler()
	{
		Vector3 v;
		ToEuler(v, *this);
		return v;
	}
	void Quaternion::FromEuler(const Vector3& v)
	{
		FromEuler(*this, v);
	}
}