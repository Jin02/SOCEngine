#include "Matrix.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

namespace Math
{
	Matrix::Matrix(void)
	{

	}

	Matrix::~Matrix(void)
	{

	}

	bool Matrix::operator != (const Matrix& mat) const
	{
		for(int i=0; i<4; ++i)
		{
			for(int j=0; j<4; ++j)
			{
				if( _m[i][j] != mat._m[i][j] )
					return true;
			}
		}

		return false;
	}

	bool Matrix::operator == (const Matrix& mat) const
	{
		return !( operator!=(mat) );
	}

	Matrix& Matrix::operator+= (const Matrix& mat)
	{
		for (int i=0; i<4; i++)
		{
			for (int j=0; j<4; j++)
			{
				_m[i][j] += mat._m[i][j];
			}
		}

		return *this;
	}

	Matrix& Matrix::operator-= (const Matrix& mat)
	{
		for (int i=0; i<4; i++)
		{
			for (int j=0; j<4; j++)
			{
				_m[i][j] -= mat._m[i][j];
			}
		}

		return *this;
	}

	Matrix& Matrix::operator*= (const Matrix& mat)
	{
		Matrix res;
		MatrixMultiply( res, (*this), mat);

		(*this) = res;

		return (*this);
	}

	Matrix& Matrix::operator*= (float f)
	{
		for(int i=0; i<4; ++i)
		{
			for(int j=0; j<4; ++j)
			{
				_m[i][j] *= f;
			}
		}

		return (*this);
	}

	Matrix& Matrix::operator/= (float f)
	{
		if( f != 0 )
			operator*=( 1.0f / f );

		return (*this);
	}

	Matrix Matrix::operator+ (const Matrix& mat) const
	{
		Matrix res;

		for(int i=0; i<4; ++i)
			for(int j=0; j<4; ++j)
				res._m[i][j] = _m[i][j] + mat._m[i][j];

		return res;
	}

	Matrix Matrix::operator- (const Matrix& mat) const
	{
		Matrix res(*this);

		for(int i=0; i<4; ++i)
			for(int j=0; j<4; ++j)
				res._m[i][j] -= mat._m[i][j];

		return res;
	}

	Matrix Matrix::operator* (const Matrix& mat)
	{
		Matrix res;

		MatrixMultiply(res, (*this), mat);

		return res;
	}

	Matrix Matrix::operator* (float f) const
	{
		Matrix res( *this );

		for(int i=0; i<4; ++i)
		{
			for(int j=0; j<4; ++j)
			{
				res._m[i][j] *= f;
			}
		}

		return res;
	}

	Matrix Matrix::operator/ (float f) const
	{
		Matrix res( *this );

		for(int i=0; i<4; ++i)
		{
			for(int j=0; j<4; ++j)
			{
				res._m[i][j] /= f;
			}
		}

		return res;
	}

	const Vector2 Matrix::operator* (const Vector2& v) const
	{
		Vector4 res(v.x, v.y, 1.0f, 1.0f);
		res = (*this) * res;

		return Vector2(res.x, res.y);
	}

	const Vector3 Matrix::operator* (const Vector3& v) const
	{
		Vector4 res(v.x, v.y, v.z, 1.0f);
		res = (*this) * res;

		return Vector3(res.x, res.y, res.z);
	}

	const Vector4 Matrix::operator* (const Vector4& v) const
	{
		Vector4 res;

		res.x = v.x + _m[0][0] + v.y * _m[1][0] + v.z * _m[2][0] + v.w * _m[3][0];
		res.y = v.x + _m[0][1] + v.y * _m[1][1] + v.z * _m[2][1] + v.w * _m[3][1];
		res.z = v.x + _m[0][2] + v.y * _m[1][2] + v.z * _m[2][2] + v.w * _m[3][2];
		res.w = v.x + _m[0][3] + v.y * _m[1][3] + v.z * _m[2][3] + v.w * _m[3][3];

		return res;
	}

	void Matrix::MatrixMultiply(Matrix& out, const Matrix& lhs, const Matrix& rhs)
	{
		for(int k=0; k<4; ++k)
		{
			for(int i=0; i<4; ++i)
			{
				float f = 0.0f;

				for(int j=0; j<4; ++j)
				{
					f += lhs._m[j][k] * rhs._m[i][j];
				}

				out._m[i][k] = f;
			}
		}
	}
}