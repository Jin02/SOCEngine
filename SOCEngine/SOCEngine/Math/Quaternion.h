#pragma once

#include "MathCommon.h"

namespace Math
{
	class Vector3;
	class Matrix;

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
		Quaternion operator + (const Quaternion& q) const;
		Quaternion operator - (const Quaternion& q) const;
		Quaternion operator * (const Quaternion& q) const;
		Quaternion operator * (float f) const;
		Quaternion operator / (float f) const;
		bool operator == (const Quaternion& q) const;
		bool operator != (const Quaternion& q) const;
		Quaternion& operator += (const Quaternion& a);
		Quaternion& operator -= (const Quaternion& a);
		Quaternion& operator *= (const Quaternion& a);

		static void Inverse(Quaternion& out, const Quaternion& in);
		static void Normalize(Quaternion& out, const Quaternion& in);
		static void Dot(float& out, const Quaternion& a, const Quaternion& b);
		static void Slerp(Quaternion& out, const Quaternion& from, const Quaternion& to, float t);
		static void Lerp(Quaternion& out, const Quaternion& from, const Quaternion& to, float t);
		static void FromEuler(Quaternion& out, const Vector3& v);

		static void FromRotationMatrix(Quaternion& out, const Matrix& m);
		static void FromAxis(Quaternion& out, const Vector3& axis, float angle);
		static void FromYawPitchRoll(Quaternion& out, float yaw, float pitch, float roll);

	public:
		void Set(float x, float y, float z, float w);
		Quaternion Normalized();
		Quaternion Inversed();
		void FromEuler(const Vector3& v);
	};

}