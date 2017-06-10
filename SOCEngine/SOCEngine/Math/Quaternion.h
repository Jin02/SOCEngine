#pragma once

namespace Math
{
	class Vector3;
	class Matrix;

	class Quaternion
	{
	public:
		Quaternion(float x = 0.0f, float y = 0.0f, float z = 0.0f, float w = 0.0f);

		const Quaternion operator + () const;
		const Quaternion operator - () const;
		const Quaternion operator + (const Quaternion& q) const;
		const Quaternion operator - (const Quaternion& q) const;
		const Quaternion operator * (const Quaternion& q) const;
		const Quaternion operator * (float f) const;
		const Quaternion operator / (float f) const;
		bool operator == (const Quaternion& q) const;
		bool operator != (const Quaternion& q) const;
		Quaternion& operator += (const Quaternion& a);
		Quaternion& operator -= (const Quaternion& a);
		Quaternion& operator *= (const Quaternion& a);

		static Quaternion Inverse(const Quaternion& in);
		static Quaternion Normalize(const Quaternion& in);
		static float Dot(const Quaternion& a, const Quaternion& b);
		static Quaternion Slerp(const Quaternion& from, const Quaternion& to, float t);
		static Quaternion Lerp(const Quaternion& from, const Quaternion& to, float t);
		static Quaternion FromEuler(const Vector3& v);

		static Quaternion FromRotationMatrix(const Matrix& m);
		static Quaternion FromAxis(const Vector3& axis, float angle);
		static Quaternion FromYawPitchRoll(float yaw, float pitch, float roll);

	public:
		void Set(float x, float y, float z, float w);
		const Quaternion Normalized();
		const Quaternion Inversed();
		const Vector3 GetForward() const;
		const Vector3 GetRight() const;
		const Vector3 GetUp() const;

	public:
		float x, y, z, w;
	};

}
