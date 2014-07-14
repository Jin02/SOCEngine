#pragma once


namespace Math
{
	class Vector2
	{
	public:
		float x;
		float y;

	public:
		Vector2(void);

		Vector2(float x, float y);

		~Vector2(void);

	public:
		Vector2 operator + () const;
		Vector2 operator - () const;

		Vector2 operator - (const Vector2 &a) const;
		Vector2 operator + (const Vector2 &a) const;
		bool operator == (const Vector2 b) const;
		bool operator != (const Vector2 &b) const;
		Vector2 operator * (float d) const;
		Vector2 operator * (const Vector2 &a) const;
		Vector2 operator / (float d) const;

		Vector2& operator += (const Vector2& a);
		Vector2& operator -= (const Vector2& a);
		Vector2& operator *= (const Vector2& a);
		Vector2& operator *= (float f);
		Vector2& operator /= (float f);

		float operator[](unsigned int idx) const;

	public:
		static Vector2 Zero();
		static Vector2 One();
		static float Angle(const Vector2 &from, const Vector2 &to, bool radian = true);
		static float Distance(const Vector2 &a, const Vector2 &b);
		static float Dot(const Vector2 &a, const Vector2 &b);
		static Vector2 Lerp(const Vector2 &from, const Vector2 &to, float t);
		static Vector2 Min(const Vector2 &a, const Vector2 &b);
		static Vector2 Max(const Vector2 &a, const Vector2 &b);
		static float Legnth(const Vector2 &a);
		static Vector2 Normalize(const Vector2 &value);
		static Vector2 Scale(const Vector2 &a, const Vector2 &b);
		static float SqrLegnth(const Vector2 &a);

	public:
		void Normalize();
		void Scale(const Vector2 &scale);
		void Set(float newX, float newY);
	};

}