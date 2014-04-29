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
		void Set( float _11, float _12, float _13, float _14,
	 			  float _21, float _22, float _23, float _24,
	  		  	  float _31, float _32, float _33, float _34,
	  		  	  float _41, float _42, float _43, float _44,
			)
		{
			m[0][0] = _11; m[0][1] = _12; m[0][2] = _13; m[0][3] = _14;
			m[1][0] = _21; m[1][1] = _22; m[1][2] = _23; m[1][3] = _24;
			m[2][0] = _31; m[2][1] = _32; m[2][2] = _33; m[2][3] = _34;
			m[3][0] = _41; m[3][1] = _42; m[3][2] = _43; m[3][3] = _44;
		}

		static void Multiply(Matrix& out, const Matrix& lhs, const Matrix& rhs);

		static void Transpose(Matrix& out, const Matrix& mat)
		{
			for(int i=0; i<4; ++i)
				for(int j=0; j<4; ++j)
					out._m[j][i] = mat._m[i][j];
		}

		const Matrix& Transpose()
		{
			Matrix::Transpose((*this), (*this));
			return (*this);
		}

		const Matrix& Inverse()
		{
			Matrix::Inverse((*this), (*this));
			return (*this);
		}

		const Matrix& Identity()
		{
			memset( &_11, 0, sizeof(float) * 16 );
			_11 = _22 = _33 = _44 = 1.0f;

			return *this;
		}

		const Matrix& Zero()
		{
			memset( &_11, 0, sizeof(float) * 16 );
			return *this;			
		}

		//using cramer's rule
		static void Inverse(Matrix& out, const Matrix& mat)
		{			
			  float tmp[12]; // Temp array for pairs
			  float src[16]; // Array of transpose source matrix
			  float det; // Determinant

			  // Transpose mat._m
			  for (int i = 0; i < 4; i++) {
			    src[i] = mat._m[i*4];
			    src[i + 4] = mat._m[i*4 + 1];
			    src[i + 8] = mat._m[i*4 + 2];
			    src[i + 12] = mat._m[i*4 + 3];
			  }

			  // Calculate pairs for first 8 elements (cofactors)
			  tmp[0] = src[10] * src[15];
			  tmp[1] = src[11] * src[14];
			  tmp[2] = src[9] * src[15];
			  tmp[3] = src[11] * src[13];
			  tmp[4] = src[9] * src[14];
			  tmp[5] = src[10] * src[13];
			  tmp[6] = src[8] * src[15];
			  tmp[7] = src[11] * src[12];
			  tmp[8] = src[8] * src[14];
			  tmp[9] = src[10] * src[12];
			  tmp[10] = src[8] * src[13];
			  tmp[11] = src[9] * src[12];

			  // Calculate first 8 elements (cofactors)
			  out._m[0] = tmp[0]*src[5] + tmp[3]*src[6] + tmp[4]*src[7];
			  out._m[0] -= tmp[1]*src[5] + tmp[2]*src[6] + tmp[5]*src[7];
			  out._m[1] = tmp[1]*src[4] + tmp[6]*src[6] + tmp[9]*src[7];
			  out._m[1] -= tmp[0]*src[4] + tmp[7]*src[6] + tmp[8]*src[7];
			  out._m[2] = tmp[2]*src[4] + tmp[7]*src[5] + tmp[10]*src[7];
			  out._m[2] -= tmp[3]*src[4] + tmp[6]*src[5] + tmp[11]*src[7];
			  out._m[3] = tmp[5]*src[4] + tmp[8]*src[5] + tmp[11]*src[6];
			  out._m[3] -= tmp[4]*src[4] + tmp[9]*src[5] + tmp[10]*src[6];
			  out._m[4] = tmp[1]*src[1] + tmp[2]*src[2] + tmp[5]*src[3];
			  out._m[4] -= tmp[0]*src[1] + tmp[3]*src[2] + tmp[4]*src[3];
			  out._m[5] = tmp[0]*src[0] + tmp[7]*src[2] + tmp[8]*src[3];
			  out._m[5] -= tmp[1]*src[0] + tmp[6]*src[2] + tmp[9]*src[3];
			  out._m[6] = tmp[3]*src[0] + tmp[6]*src[1] + tmp[11]*src[3];
			  out._m[6] -= tmp[2]*src[0] + tmp[7]*src[1] + tmp[10]*src[3];
			  out._m[7] = tmp[4]*src[0] + tmp[9]*src[1] + tmp[10]*src[2];
			  out._m[7] -= tmp[5]*src[0] + tmp[8]*src[1] + tmp[11]*src[2];

			  // Calculate pairs for second 8 elements (cofactors)
			  tmp[0] = src[2]*src[7];
			  tmp[1] = src[3]*src[6];
			  tmp[2] = src[1]*src[7];
			  tmp[3] = src[3]*src[5];
			  tmp[4] = src[1]*src[6];
			  tmp[5] = src[2]*src[5];
			  tmp[6] = src[0]*src[7];
			  tmp[7] = src[3]*src[4];
			  tmp[8] = src[0]*src[6];
			  tmp[9] = src[2]*src[4];
			  tmp[10] = src[0]*src[5];
			  tmp[11] = src[1]*src[4];

			  // Calculate second 8 elements (cofactors)
			  out._m[8] = tmp[0]*src[13] + tmp[3]*src[14] + tmp[4]*src[15];
			  out._m[8] -= tmp[1]*src[13] + tmp[2]*src[14] + tmp[5]*src[15];
			  out._m[9] = tmp[1]*src[12] + tmp[6]*src[14] + tmp[9]*src[15];
			  out._m[9] -= tmp[0]*src[12] + tmp[7]*src[14] + tmp[8]*src[15];
			  out._m[10] = tmp[2]*src[12] + tmp[7]*src[13] + tmp[10]*src[15];
			  out._m[10]-= tmp[3]*src[12] + tmp[6]*src[13] + tmp[11]*src[15];
			  out._m[11] = tmp[5]*src[12] + tmp[8]*src[13] + tmp[11]*src[14];
			  out._m[11]-= tmp[4]*src[12] + tmp[9]*src[13] + tmp[10]*src[14];
			  out._m[12] = tmp[2]*src[10] + tmp[5]*src[11] + tmp[1]*src[9];
			  out._m[12]-= tmp[4]*src[11] + tmp[0]*src[9] + tmp[3]*src[10];
			  out._m[13] = tmp[8]*src[11] + tmp[0]*src[8] + tmp[7]*src[10];
			  out._m[13]-= tmp[6]*src[10] + tmp[9]*src[11] + tmp[1]*src[8];
			  out._m[14] = tmp[6]*src[9] + tmp[11]*src[11] + tmp[3]*src[8];
			  out._m[14]-= tmp[10]*src[11] + tmp[2]*src[8] + tmp[7]*src[9];
			  out._m[15] = tmp[10]*src[10] + tmp[4]*src[8] + tmp[9]*src[9];
			  out._m[15]-= tmp[8]*src[9] + tmp[11]*src[10] + tmp[5]*src[8];

			  // Calculate determinant
			  det=src[0]*out._m[0]+src[1]*out._m[1]+src[2]*out._m[2]+src[3]*out._m[3];

			  // Calculate matrix inverse
			  det = 1/det;

			  for (int j = 0; j < 16; j++) {
			    out._m[j] *= det;
			  }
		}


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