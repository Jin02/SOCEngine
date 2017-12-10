#pragma once

namespace Math
{
	class Matrix;

	class Vector3
	{
	public:
		Vector3(void) = default;
		Vector3(float x, float y, float z);

	public:
		const Vector3 operator + () const;
		const Vector3 operator - () const;

		const Vector3 operator - (const Vector3& a) const;
		const Vector3 operator + (const Vector3& a) const;
		bool operator == (const Vector3& b) const;
		bool operator != (const Vector3& b) const;
		const Vector3 operator * (float d) const;
		const Vector3 operator * (const Vector3& a) const;
		const Vector3 operator / (float d) const;

		Vector3& operator += (const Vector3& a);
		Vector3& operator -= (const Vector3& a);
		Vector3& operator *= (const Vector3& a);
		Vector3& operator *= (float f);
		Vector3& operator /= (float f);

		float operator[](unsigned int idx) const;

		friend Vector3 operator+(float lhs, const Vector3& rhs) { return Vector3(lhs + rhs.x, lhs + rhs.y, lhs + rhs.z); }
		friend Vector3 operator-(float lhs, const Vector3& rhs) { return Vector3(lhs - rhs.x, lhs - rhs.y, lhs - rhs.z); }
		friend Vector3 operator*(float lhs, const Vector3& rhs) { return Vector3(lhs * rhs.x, lhs * rhs.y, lhs * rhs.z); }
		friend Vector3 operator/(float lhs, const Vector3& rhs) { return Vector3(lhs / rhs.x, lhs / rhs.y, lhs / rhs.z); }


	public:
		static const Vector3 Forward();
		static const Vector3 Right();
		static const Vector3 One();
		static const Vector3 Up();
		static const Vector3 Zero();
		static const Vector3 Cross(const Vector3& a, const Vector3& b);
		static const Vector3 Lerp(const Vector3& from, const Vector3& to, float t);
		static const Vector3 Min(const Vector3& a, const Vector3& b);
		static const Vector3 Max(const Vector3& a, const Vector3& b);
		static const Vector3 Normalize(const Vector3 &value);
		static const Vector3 Project(const Vector3& vector, const Vector3& onNormal);
		static const Vector3 Reflect(const Vector3& inDirection, const Vector3& inNormal);

		static float AnglebyDirect(const Vector3& from, const Vector3& to, bool radian = true);
		static float AnglebyPoint(const Vector3& from, const Vector3& to, bool radian = true);
		static float Distance(const Vector3& a, const Vector3& b);
		static float Dot(const Vector3& a, const Vector3& b);
		static float Length(const Vector3& a);
		static float SqrLegnth(const Vector3& a);

		static Vector3 TransformCoord(const Vector3& v, const Matrix& mat);
		static Vector3 TransformNormal(const Vector3& v, const Matrix& mat);
		static Vector3 FromRotationMatrix(const Matrix& rotMat);
		static Vector3 EulerNormalize(const Vector3& euler);

	public:
		const Vector3 Normalized() const;

		float Length() const;
		float Dot(const Vector3& v) const;
		void Set(float newX, float newY, float newZ);

	public:
		union
		{
			struct
			{
				float x, y, z;
			};

			float value[3] = {0.0f, };
		};
	};
}