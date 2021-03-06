#pragma once


namespace Math
{
	class Vector2
	{
	public:
		Vector2() = default;
		Vector2(float x, float y);

	public:
		const Vector2 operator + () const;
		const Vector2 operator - () const;

		const Vector2 operator - (const Vector2 &a) const;
		const Vector2 operator + (const Vector2 &a) const;
		bool operator == (const Vector2 b) const;
		bool operator != (const Vector2 &b) const;
		const Vector2 operator * (float d) const;
		const Vector2 operator * (const Vector2 &a) const;
		const Vector2 operator / (float d) const;

		Vector2& operator += (const Vector2& a);
		Vector2& operator -= (const Vector2& a);
		Vector2& operator *= (const Vector2& a);
		Vector2& operator *= (float f);
		Vector2& operator /= (float f);

		float operator[](unsigned int idx) const;

		friend Vector2 operator+(float lhs, const Vector2& rhs) { return Vector2(lhs + rhs.x, lhs + rhs.y); }
		friend Vector2 operator-(float lhs, const Vector2& rhs) { return Vector2(lhs - rhs.x, lhs - rhs.y); }
		friend Vector2 operator*(float lhs, const Vector2& rhs) { return Vector2(lhs * rhs.x, lhs * rhs.y); }
		friend Vector2 operator/(float lhs, const Vector2& rhs) { return Vector2(lhs / rhs.x, lhs / rhs.y); }

	public:
		static const Vector2 Zero();
		static const Vector2 One();
		static float AnglebyPoint(const Vector2 &from, const Vector2 &to, bool radian = true);
        static float AnglebyDirect(const Vector2 &from, const Vector2 &to, bool radian = true);
		static float Distance(const Vector2 &a, const Vector2 &b);
		static float Dot(const Vector2 &a, const Vector2 &b);
		static const Vector2 Lerp(const Vector2 &from, const Vector2 &to, float t);
		static const Vector2 Min(const Vector2 &a, const Vector2 &b);
		static const Vector2 Max(const Vector2 &a, const Vector2 &b);
		static float Length(const Vector2 &a);
		static const Vector2 Normalize(const Vector2 &value);
		static float SqrLength(const Vector2 &a);

	public:
		const Vector2 Normalized() const;
		void Set(float newX, float newY);

	public:
		union
		{
			struct
			{
				float x, y;
			};

			float value[2] = {0.0f };
		};
	};

}