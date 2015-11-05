#pragma once



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
		Vector4 operator + () const;
		Vector4 operator - () const;

		Vector4 operator - (const Vector4& a) const;
		Vector4 operator + (const Vector4& a) const;
		bool operator == (const Vector4& b) const;
		bool operator != (const Vector4& b) const;
		Vector4 operator * (float d) const;
		Vector4 operator / (float d) const;

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
		static Vector4 Lerp(const Vector4& from, const Vector4& to, float t);
		static Vector4 Min(const Vector4& a, const Vector4& b);
		static Vector4 Max(const Vector4& a, const Vector4& b);
		static float Legnth(const Vector4& a);
		static Vector4 Normalize(const Vector4& value);
		static float SqrLegnth(const Vector4& a);

	public:
		Vector4 Normalized() const;
		void Set(float newX, float newY, float newZ, float newW);
	};

}