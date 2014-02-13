#pragma once

#include "Vector4.h"

namespace Math
{

	class Vector2;
	class Vector3;
	class Vector4;


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
		void MatrixMultiply(Matrix& out, const Matrix& lhs, const Matrix& rhs);
		//inverse, transpose, other....

	private:
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