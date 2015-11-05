#pragma once

namespace Math
{
	class Matrix;

	class Vector3
	{
	public:
		float x;
		float y;
		float z;

	public:
		Vector3();
		Vector3(float x, float y);
		Vector3(float x, float y, float z);
		~Vector3(void);

	public:
		Vector3 operator + () const;
		Vector3 operator - () const;

		Vector3 operator - (const Vector3& a) const;
		Vector3 operator + (const Vector3& a) const;
		bool operator == (const Vector3& b) const;
		bool operator != (const Vector3& b) const;
		Vector3 operator * (float d) const;
		Vector3 operator * (const Vector3& a) const;
		Vector3 operator / (float d) const;

		Vector3& operator += (const Vector3& a);
		Vector3& operator -= (const Vector3& a);
		Vector3& operator *= (const Vector3& a);
		Vector3& operator *= (float f);
		Vector3& operator /= (float f);

		float operator[](unsigned int idx) const;

	public:
		static Vector3 Forward();
		static Vector3 Right();
		static Vector3 One();
		static Vector3 Up();
		static Vector3 Zero();
		static float AnglebyDirect(const Vector3& from, const Vector3& to, bool radian = true);
		static float AnglebyPoint(const Vector3& from, const Vector3& to, bool radian = true);
		static Vector3 Cross(const Vector3& a, const Vector3& b);
		static float Distance(const Vector3& a, const Vector3& b);
		static float Dot(const Vector3& a, const Vector3& b);
		static Vector3 Lerp(const Vector3& from, const Vector3& to, float t);
		static Vector3 Min(const Vector3& a, const Vector3& b);
		static Vector3 Max(const Vector3& a, const Vector3& b);
		static float Length(const Vector3& a);
		static Vector3 Normalize(const Vector3 &value);
		static Vector3 Project(const Vector3& vector, const Vector3& onNormal);
		static Vector3 Reflect(const Vector3& inDirection, const Vector3& inNormal);
		static float SqrLegnth(const Vector3& a);
		static void TransformCoord(Vector3& out, const Vector3& v, const Matrix& mat);
		static void TransformNormal(Vector3& out, const Vector3& v, const Matrix& mat);
		static void FromRotationMatrix(Vector3& out, const Matrix& rotMat);

	public:
		Vector3 Normalized() const;

		float Length() const;
		float Dot(const Vector3& v) const;
		void Set(float newX, float newY, float newZ);
	};
}