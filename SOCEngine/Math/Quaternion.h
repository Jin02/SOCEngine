#pragma once

namespace Math
{
	class Quaternion
	{
	public:
		float x, y, z, w;

	public:
		Quaternion() : x(0.0f), y(0.0f), z(0.0f), w(0.0f)
		{
		}

		Quaternion(float x, float y, float z, float w)
		{
			this->x = x;
			this->y = y;
			this->z = z;
			this->w = w;
		}

		~Quaternion()
		{
		}

	public:
		Quaternion operator + () const
		{

		}
		Quaternion operator - () const
		{

		}

		Quaternion operator + (const Quaternion& rhs) const
		{

		}
		Quaternion operator - (const Quaternion& rhs) const
		{

		}
		Quaternion operator * (const Quaternion& rhs) const
		{
			Quaternion q;
			q.x = (w * rhs.x) + (x * rhs.w) + (y * rhs.z) - (z * rhs.y);
			q.y = (w * rhs.y) + (y * rhs.w) + (z * rhs.x) - (x * rhs.z);
			q.z = (w * rhs.z) + (z * rhs.w) + (x * rhs.y) - (y * rhs.x);
			q.w = (w * rhs.w) - (x * rhs.x) - (y * rhs.y) - (z * rhs.z);

			return q;
		}
		Quaternion operator * (float f) const
		{

		}
		Quaternion operator / (float f) const
		{

		}
		bool operator == (const Quaternion& rhs) const
		{

		}
		bool operator != (const Quaternion& rhs) const
		{

		}

		Quaternion& operator += (const Quaternion& a)
		{

		}
		Quaternion& operator -= (const Quaternion& a)
		{
		}

		Quaternion& operator *= (const Quaternion& a)
		{
		}

	public:
		void Set(float x,  float y, float z, float w)
		{
			this->x = x;
			this->y = y;
			this->z = z;
			this->w = w;
		}
	};

}