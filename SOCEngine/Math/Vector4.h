#pragma once

#include "Common.h"

namespace Math
{
	class Vector4
	{
	public:
		float x;
		float y;
		float z;
		float w;

	public:
		Vector4(void);
		Vector4(float x, float y, float z);
		Vector4(float x, float y, float z, float w);
		~Vector4(void);

	public:
		Vector4 operator - (const Vector4& a) const;
		Vector4 operator + (const Vector4& a) const;
		bool operator == (const Vector4& rhs) const;
		bool operator != (const Vector4& rhs) const;
		Vector4 operator * (float d) const;
		Vector4 operator * (const Vector4& a) const;
		Vector4 operator / (float d) const;

		Vector4& operator += (const Vector4& a);
		Vector4& operator -= (const Vector4& a);
		Vector4& operator *= (const Vector4& a);
		Vector4& operator *= (float f);
		Vector4& operator /= (float f);

	public:
		static Vector4 One();
		static Vector4 Zero();

	public:
		static float Distance(const Vector4& a, const Vector4& b);
		static float Dot(const Vector4& lhs, const Vector4& rhs);
		static Vector4 Lerp(const Vector4& from, const Vector4& to, float t);
		static Vector4 Min(const Vector4& lhs, const Vector4& rhs);
		static Vector4 Max(const Vector4& lhs, const Vector4& rhs);
		static float Legnth(const Vector4& a);
		static Vector4 Normalize(Vector4& value);
		static Vector4 Scale(const Vector4& a, const Vector4& b);
		static float SqrLegnth(const Vector4& a);

	public:
		void Normalize();
		void Scale(const Vector4& scale);
		void Set(float newX, float newY, float newZ, float newW);
	};

}