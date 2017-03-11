#pragma once



namespace Math
{
	class Vector4
	{
	public:
		Vector4(float x = 0.0f, float y = 0.0f, float z = 0.0f, float w = 0.0f);

	public:
		const Vector4 operator + () const;
		const Vector4 operator - () const;

		const Vector4 operator - (const Vector4& a) const;
		const Vector4 operator + (const Vector4& a) const;
		bool operator == (const Vector4& b) const;
		bool operator != (const Vector4& b) const;
		const Vector4 operator * (float d) const;
		const Vector4 operator / (float d) const;

		Vector4& operator += (const Vector4& a);
		Vector4& operator -= (const Vector4& a);
		Vector4& operator *= (const Vector4& a);
		Vector4& operator *= (float f);
		Vector4& operator /= (float f);

		float operator[](unsigned int idx) const;

	public:
		static Vector4 One();
		static Vector4 Zero();

	public:
		static float Distance(const Vector4& a, const Vector4& b);
		static float Dot(const Vector4& a, const Vector4& b);
		static const Vector4 Lerp(const Vector4& from, const Vector4& to, float t);
		static const Vector4 Min(const Vector4& a, const Vector4& b);
		static const Vector4 Max(const Vector4& a, const Vector4& b);
		static float Legnth(const Vector4& a);
		static const Vector4 Normalize(const Vector4& value);
		static float SqrLegnth(const Vector4& a);

	public:
		const Vector4 Normalized() const;
		void Set(float newX, float newY, float newZ, float newW);

	public:
		float x, y, z, w;
	};

}