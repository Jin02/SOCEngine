#pragma once

#include "Common.h"
#include "Rect.h"

namespace Math
{
	class Vector2;
	class Vector3;
	class Vector4;
	class Quaternion;

	class Matrix
	{
	public:
		Matrix() = default;
		Matrix(	float _11, float _12, float _13, float _14,
				float _21, float _22, float _23, float _24,
				float _31, float _32, float _33, float _34,
				float _41, float _42, float _43, float _44	);

	public:
		bool operator != (const Matrix& mat) const;
		bool operator == (const Matrix& mat) const;

		Matrix& operator+= (const Matrix& mat);
		Matrix& operator-= (const Matrix& mat);
		Matrix& operator*= (const Matrix& mat);
		Matrix& operator*= (float f);
		Matrix& operator/= (float f);

		const Matrix operator+ (const Matrix& mat) const;
		const Matrix operator- (const Matrix& mat) const;
		const Matrix operator* (const Matrix& mat);
		const Matrix operator* (float f) const;
		const Matrix operator/ (float f) const;
		const Matrix operator* (const Matrix& mat) const;

		const Vector2 operator* (const Vector2& v) const;
		const Vector3 operator* (const Vector3& v) const;
		const Vector4 operator* (const Vector4& v) const;

	public:
		static Matrix RotateUsingQuaternion(const Quaternion& q);
		static Matrix PerspectiveFovLH(float aspect, float fovy, float zn, float zf);
		static Matrix OrthoLH(float w, float h, float zn, float zf);
		static Matrix RotateUsingAxis(const Vector3& v, float angle);
		static Matrix ComputeViewMatrix(const Matrix &worldMatrix);
		static Matrix ComputeViewportMatrix(const Rect<uint>& rect);
		static Matrix ComputeInvViewportMatrix(const Rect<uint>& rect);
		static Matrix MakeRotationMatrix(const Vector3& right, const Vector3& up, const Vector3& forward);
		static Matrix LookAtDir(const Vector3 & targetDir, const Vector3* upVec = nullptr);

	public:
		void Set(	float _11, float _12, float _13, float _14,
					float _21, float _22, float _23, float _24,
					float _31, float _32, float _33, float _34,
					float _41, float _42, float _43, float _44 );

		static Matrix Multiply(const Matrix& lhs, const Matrix& rhs);
		static Matrix Transpose(const Matrix& mat);
		static Matrix Inverse(const Matrix& mat); 		//using cramer's rule
		static Matrix Identity();

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

			float _m[4][4] = { 0.0f, };
		};
	};

}