#pragma once

namespace Math
{
	class Vector2;
	class Vector3;
	class Vector4;
	class Quaternion;

	class Matrix
	{
	public:
		Matrix(void);
		~Matrix(void);

	public:
		bool operator != (const Matrix& mat) const;
		bool operator == (const Matrix& mat) const;

		Matrix& operator+= (const Matrix& mat);
		Matrix& operator-= (const Matrix& mat);
		Matrix& operator*= (const Matrix& mat);
		Matrix& operator*= (float f);
		Matrix& operator/= (float f);

		Matrix operator+ (const Matrix& mat) const;
		Matrix operator- (const Matrix& mat) const;
		Matrix operator* (const Matrix& mat);
		Matrix operator* (float f) const;
		Matrix operator/ (float f) const;

		const Vector2 operator* (const Vector2& v) const;
		const Vector3 operator* (const Vector3& v) const;
		const Vector4 operator* (const Vector4& v) const;

	public:
		static void RotationQuaternion(Matrix& out, const Quaternion& q);
		static void PerspectiveFovLH(Matrix& out, float aspect, float fovy, float zn, float zf);
		static void OrthoLH(Matrix& out, float w, float h, float zn, float zf);
		static void RotationAxis(Matrix& out, const Vector3& v, float angle);

	public:
		void Set( float _11, float _12, float _13, float _14,
			float _21, float _22, float _23, float _24,
			float _31, float _32, float _33, float _34,
			float _41, float _42, float _43, float _44 );

		static void Multiply(Matrix& out, const Matrix& lhs, const Matrix& rhs);
		static void Transpose(Matrix& out, const Matrix& mat);
		static void Inverse(Matrix& out, const Matrix& mat); 		//using cramer's rule
		static void Identity(Matrix& out);

	public:
		union
		{
			struct
			{
				float        _11, _12, _13, _14;
				float        _21, _22, _23, _24;
				float        _31, _32, _33, _34;
				float        _41, _42, _43, _44;
			};

			float _m[4][4];
		};
	};

}