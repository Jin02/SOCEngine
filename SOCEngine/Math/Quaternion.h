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