#pragma once

#include "Common.h"

namespace Math
{
	class Vector3;

	class Quaternion
	{
	public:
		float x, y, z, w;

	public:
		Quaternion();
		Quaternion(float x, float y, float z, float w);
		~Quaternion();

	public:
		Quaternion operator + () const;
		Quaternion operator - () const;
		Quaternion operator + (const Quaternion& rhs) const;
		Quaternion operator - (const Quaternion& rhs) const;
		Quaternion operator * (const Quaternion& rhs) const;
		Quaternion operator * (float f) const;
		Quaternion operator / (float f) const;
		bool operator == (const Quaternion& rhs) const;
		bool operator != (const Quaternion& rhs) const;
		Quaternion& operator += (const Quaternion& a);
		Quaternion& operator -= (const Quaternion& a);
		Quaternion& operator *= (const Quaternion& a);

		static void Inverse(Quaternion& out, const Quaternion& in);
		static void Normalize(Quaternion& out, const Quaternion& in);
		static void Dot(float& out, const Quaternion& a, const Quaternion& b);
		static void Slerp(Quaternion& out, const Quaternion& from, const Quaternion& to, float t);
		static void Lerp(Quaternion& out, const Quaternion& from, const Quaternion& to, float t);
		static void ToEuler(Vector3& out, const Quaternion& q);
		static void FromEuler(Quaternion& out, const Vector3& v);

	public:
		void Set(float x,  float y, float z, float w);
	};

}