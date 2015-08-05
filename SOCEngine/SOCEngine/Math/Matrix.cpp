#include "Matrix.h"

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Quaternion.h"

#include "MathCommon.h"

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
			for(int j=0; j<4; ++j)
			{
				float f = abs( _m[i][j] - mat._m[i][j] );

				if( f > EPSILON )
					return true;
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
			for (int j=0; j<4; j++)
				_m[i][j] += mat._m[i][j];

		return *this;
	}

	Matrix& Matrix::operator-= (const Matrix& mat)
	{
		for (int i=0; i<4; i++)
			for (int j=0; j<4; j++)
				_m[i][j] -= mat._m[i][j];

		return *this;
	}

	Matrix& Matrix::operator*= (const Matrix& mat)
	{
		Matrix res;
		Multiply( res, (*this), mat);

		(*this) = res;

		return (*this);
	}

	Matrix& Matrix::operator*= (float f)
	{
		for(int i=0; i<4; ++i)
			for(int j=0; j<4; ++j)
				_m[i][j] *= f;

		return (*this);
	}

	Matrix& Matrix::operator/= (float f)
	{
		return operator*=( 1.0f / f );
	}

	Matrix Matrix::operator+ (const Matrix& mat) const
	{
		Matrix res(*this);

		for(int i=0; i<4; ++i)
			for(int j=0; j<4; ++j)
				res._m[i][j] += mat._m[i][j];

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
		Multiply(res, (*this), mat);

		return res;
	}

	Matrix Matrix::operator* (float f) const
	{
		Matrix res( *this );

		for(int i=0; i<4; ++i)
			for(int j=0; j<4; ++j)
				res._m[i][j] *= f;

		return res;
	}

	Matrix Matrix::operator/ (float f) const
	{
		Matrix res( *this );

		for(int i=0; i<4; ++i)
			for(int j=0; j<4; ++j)
				res._m[i][j] /= f;

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

		res.x = v.x * _m[0][0] + v.y * _m[1][0] + v.z * _m[2][0] + v.w * _m[3][0];
		res.y = v.x * _m[0][1] + v.y * _m[1][1] + v.z * _m[2][1] + v.w * _m[3][1];
		res.z = v.x * _m[0][2] + v.y * _m[1][2] + v.z * _m[2][2] + v.w * _m[3][2];
		res.w = v.x * _m[0][3] + v.y * _m[1][3] + v.z * _m[2][3] + v.w * _m[3][3];

		return res;
	}

	void Matrix::Transpose(Matrix& out, const Matrix& mat)
	{
		Matrix origin = mat;

		out._12 = origin._21;
		out._13 = origin._31;
		out._14 = origin._41;

		out._21 = origin._12;
		out._23 = origin._32;
		out._24 = origin._42;

		out._31 = origin._13;
		out._32 = origin._23;
		out._34 = origin._43;

		out._41 = origin._14;
		out._42 = origin._24;
		out._43 = origin._34;
	}

	void Matrix::Set( float _11, float _12, float _13, float _14,
		float _21, float _22, float _23, float _24,
		float _31, float _32, float _33, float _34,
		float _41, float _42, float _43, float _44
		)
	{
		_m[0][0] = _11; _m[0][1] = _12; _m[0][2] = _13; _m[0][3] = _14;
		_m[1][0] = _21; _m[1][1] = _22; _m[1][2] = _23; _m[1][3] = _24;
		_m[2][0] = _31; _m[2][1] = _32; _m[2][2] = _33; _m[2][3] = _34;
		_m[3][0] = _41; _m[3][1] = _42; _m[3][2] = _43; _m[3][3] = _44;
	}

	void Matrix::Multiply(Matrix& out, const Matrix& lhs, const Matrix& rhs)
	{
		for(int i=0; i<4; ++i)
		{
			for(int j=0; j<4; ++j)
			{
				float f = 0.0f;
				for(int k=0; k<4; ++k)
				{
					f += lhs._m[i][k] * rhs._m[k][j];
				}

				out._m[i][j] = f;
			}
		}
	}

	void Matrix::Identity(Matrix& out)
	{
		memset(&out, 0, sizeof(Matrix));
		out._11 = out._22 = out._33 = out._44 = 1.0f;
	}

	void Matrix::Inverse(Matrix& out, const Matrix& mat)
	{			
		Matrix src;
		Matrix::Transpose(src, mat);

		float tmpCofactors[12];
		/* calculate pairs for first 8 elements (cofactors) */
		tmpCofactors[0] = src._m[2][2] * src._m[3][3];
		tmpCofactors[1] = src._m[2][3] * src._m[3][2];
		tmpCofactors[2] = src._m[2][1] * src._m[3][3];
		tmpCofactors[3] = src._m[2][3] * src._m[3][1];
		tmpCofactors[4] = src._m[2][1] * src._m[3][2];
		tmpCofactors[5] = src._m[2][2] * src._m[3][1];
		tmpCofactors[6] = src._m[2][0] * src._m[3][3];
		tmpCofactors[7] = src._m[2][3] * src._m[3][0];
		tmpCofactors[8] = src._m[2][0] * src._m[3][2];
		tmpCofactors[9] = src._m[2][2] * src._m[3][0];
		tmpCofactors[10] = src._m[2][0] * src._m[3][1];
		tmpCofactors[11] = src._m[2][1] * src._m[3][0];

		/* calculate first 8 elements (cofactors) */
		out._m[0][0] = tmpCofactors[0]*src._m[1][1] + tmpCofactors[3]*src._m[1][2] + tmpCofactors[4]*src._m[1][3];
		out._m[0][0] -= tmpCofactors[1]*src._m[1][1] + tmpCofactors[2]*src._m[1][2] + tmpCofactors[5]*src._m[1][3];
		out._m[0][1] = tmpCofactors[1]*src._m[1][0] + tmpCofactors[6]*src._m[1][2] + tmpCofactors[9]*src._m[1][3];
		out._m[0][1] -= tmpCofactors[0]*src._m[1][0] + tmpCofactors[7]*src._m[1][2] + tmpCofactors[8]*src._m[1][3];
		out._m[0][2] = tmpCofactors[2]*src._m[1][0] + tmpCofactors[7]*src._m[1][1] + tmpCofactors[10]*src._m[1][3];
		out._m[0][2] -= tmpCofactors[3]*src._m[1][0] + tmpCofactors[6]*src._m[1][1] + tmpCofactors[11]*src._m[1][3];
		out._m[0][3] = tmpCofactors[5]*src._m[1][0] + tmpCofactors[8]*src._m[1][1] + tmpCofactors[11]*src._m[1][2];
		out._m[0][3] -= tmpCofactors[4]*src._m[1][0] + tmpCofactors[9]*src._m[1][1] + tmpCofactors[10]*src._m[1][2];
		out._m[1][0] = tmpCofactors[1]*src._m[0][1] + tmpCofactors[2]*src._m[0][2] + tmpCofactors[5]*src._m[0][3];
		out._m[1][0] -= tmpCofactors[0]*src._m[0][1] + tmpCofactors[3]*src._m[0][2] + tmpCofactors[4]*src._m[0][3];
		out._m[1][1] = tmpCofactors[0]*src._m[0][0] + tmpCofactors[7]*src._m[0][2] + tmpCofactors[8]*src._m[0][3];
		out._m[1][1] -= tmpCofactors[1]*src._m[0][0] + tmpCofactors[6]*src._m[0][2] + tmpCofactors[9]*src._m[0][3];
		out._m[1][2] = tmpCofactors[3]*src._m[0][0] + tmpCofactors[6]*src._m[0][1] + tmpCofactors[11]*src._m[0][3];
		out._m[1][2] -= tmpCofactors[2]*src._m[0][0] + tmpCofactors[7]*src._m[0][1] + tmpCofactors[10]*src._m[0][3];
		out._m[1][3] = tmpCofactors[4]*src._m[0][0] + tmpCofactors[9]*src._m[0][1] + tmpCofactors[10]*src._m[0][2];
		out._m[1][3] -= tmpCofactors[5]*src._m[0][0] + tmpCofactors[8]*src._m[0][1] + tmpCofactors[11]*src._m[0][2];

		/* calculate pairs for second 8 elements (cofactors) */
		tmpCofactors[0] = src._m[0][2]*src._m[1][3];
		tmpCofactors[1] = src._m[0][3]*src._m[1][2];
		tmpCofactors[2] = src._m[0][1]*src._m[1][3];
		tmpCofactors[3] = src._m[0][3]*src._m[1][1];
		tmpCofactors[4] = src._m[0][1]*src._m[1][2];
		tmpCofactors[5] = src._m[0][2]*src._m[1][1];
		tmpCofactors[6] = src._m[0][0]*src._m[1][3];
		tmpCofactors[7] = src._m[0][3]*src._m[1][0];
		tmpCofactors[8] = src._m[0][0]*src._m[1][2];
		tmpCofactors[9] = src._m[0][2]*src._m[1][0];
		tmpCofactors[10] = src._m[0][0]*src._m[1][1];
		tmpCofactors[11] = src._m[0][1]*src._m[1][0];

		/* calculate second 8 elements (cofactors) */
		out._m[2][0] = tmpCofactors[0]*src._m[3][1] + tmpCofactors[3]*src._m[3][2] + tmpCofactors[4]*src._m[3][3];
		out._m[2][0] -= tmpCofactors[1]*src._m[3][1] + tmpCofactors[2]*src._m[3][2] + tmpCofactors[5]*src._m[3][3];
		out._m[2][1] = tmpCofactors[1]*src._m[3][0] + tmpCofactors[6]*src._m[3][2] + tmpCofactors[9]*src._m[3][3];
		out._m[2][1] -= tmpCofactors[0]*src._m[3][0] + tmpCofactors[7]*src._m[3][2] + tmpCofactors[8]*src._m[3][3];
		out._m[2][2] = tmpCofactors[2]*src._m[3][0] + tmpCofactors[7]*src._m[3][1] + tmpCofactors[10]*src._m[3][3];
		out._m[2][2]-= tmpCofactors[3]*src._m[3][0] + tmpCofactors[6]*src._m[3][1] + tmpCofactors[11]*src._m[3][3];
		out._m[2][3] = tmpCofactors[5]*src._m[3][0] + tmpCofactors[8]*src._m[3][1] + tmpCofactors[11]*src._m[3][2];
		out._m[2][3]-= tmpCofactors[4]*src._m[3][0] + tmpCofactors[9]*src._m[3][1] + tmpCofactors[10]*src._m[3][2];
		out._m[3][0] = tmpCofactors[2]*src._m[2][2] + tmpCofactors[5]*src._m[2][3] + tmpCofactors[1]*src._m[2][1];
		out._m[3][0]-= tmpCofactors[4]*src._m[2][3] + tmpCofactors[0]*src._m[2][1] + tmpCofactors[3]*src._m[2][2];
		out._m[3][1] = tmpCofactors[8]*src._m[2][3] + tmpCofactors[0]*src._m[2][0] + tmpCofactors[7]*src._m[2][2];
		out._m[3][1]-= tmpCofactors[6]*src._m[2][2] + tmpCofactors[9]*src._m[2][3] + tmpCofactors[1]*src._m[2][0];
		out._m[3][2] = tmpCofactors[6]*src._m[2][1] + tmpCofactors[11]*src._m[2][3] + tmpCofactors[3]*src._m[2][0];
		out._m[3][2]-= tmpCofactors[10]*src._m[2][3] + tmpCofactors[2]*src._m[2][0] + tmpCofactors[7]*src._m[2][1];
		out._m[3][3] = tmpCofactors[10]*src._m[2][2] + tmpCofactors[4]*src._m[2][0] + tmpCofactors[9]*src._m[2][1];
		out._m[3][3]-= tmpCofactors[8]*src._m[2][1] + tmpCofactors[11]*src._m[2][2] + tmpCofactors[5]*src._m[2][0];

		/* calculate determinant */
		float det = src._m[0][0]*out._m[0][0]+src._m[0][1]*out._m[0][1]+src._m[0][2]*out._m[0][2]+src._m[0][3]*out._m[0][3];

		/* calculate matrix inverse */
		det = 1/det;
		for (int i = 0; i < 4; i++)
			for( int j = 0; j < 4; ++j)
				out._m[i][j] *= det;
	}

	void Matrix::RotationQuaternion(Matrix& out, const Quaternion& q)
	{
		Identity(out);

		out._m[0][0] = 1.0f - 2.0f * (q.y * q.y + q.z * q.z);
		out._m[0][1] = 2.0f * (q.x *q.y + q.z * q.w);
		out._m[0][2] = 2.0f * (q.x * q.z - q.y * q.w);
		out._m[1][0] = 2.0f * (q.x * q.y - q.z * q.w);
		out._m[1][1] = 1.0f - 2.0f * (q.x * q.x + q.z * q.z);
		out._m[1][2] = 2.0f * (q.y *q.z + q.x *q.w);
		out._m[2][0] = 2.0f * (q.x * q.z + q.y * q.w);
		out._m[2][1] = 2.0f * (q.y *q.z - q.x *q.w);
		out._m[2][2] = 1.0f - 2.0f * (q.x * q.x + q.y * q.y);
	}

	void Matrix::PerspectiveFovLH(Matrix& out, float aspect, float fovy, float zn, float zf)
	{
		Identity(out);

		float yScale = 1.0f / tanf(fovy/2.0f);
		float xScale = yScale / aspect;

		out._m[0][0] = xScale;
		out._m[1][1] = yScale;
		out._m[2][2] = zf / (zf - zn);
		out._m[2][3] = 1.0f;
		out._m[3][2] = -zn * zf / (zf - zn);
		out._m[3][3] = 0.0f;
	}

	void Matrix::OrthoLH(Matrix& out, float w, float h, float zn, float zf)
	{
		Identity(out);

		out._m[0][0] = 2.0f / w;
		out._m[1][1] = 2.0f / h;
		out._m[2][2] = 1.0f / (zf - zn);
		out._m[3][3] = 1.0f;

		out._m[3][2] = zn / (zn - zf);
	}

	void Matrix::RotationAxis(Matrix& out, const Vector3& v, float angle)
	{
		Vector3 nv = Vector3::Normalize(v);
		float sangle, cangle, cdiff;

		sangle = sinf(angle);
		cangle = cosf(angle);
		cdiff = 1.0f - cangle;

		out._m[0][0] = cdiff * nv.x * nv.x + cangle;
		out._m[0][1] = cdiff * nv.y * nv.x + sangle * nv.z;
		out._m[0][2] = cdiff * nv.z * nv.x - sangle * nv.y;
		out._m[0][3] = 0.0f;

		out._m[1][0] = cdiff * nv.x * nv.y - sangle * nv.z;
		out._m[1][1] = cdiff * nv.y * nv.y + cangle;
		out._m[1][2] = cdiff * nv.z * nv.y + sangle * nv.x;
		out._m[1][3] = 0.0f;

		out._m[2][0] = cdiff * nv.x * nv.z + sangle * nv.y;
		out._m[2][1] = cdiff * nv.y * nv.z - sangle * nv.x;
		out._m[2][2] = cdiff * nv.z * nv.z + cangle;
		out._m[2][3] = 0.0f;

		out._m[3][0] = 0.0f;
		out._m[3][1] = 0.0f;
		out._m[3][2] = 0.0f;
		out._m[3][3] = 1.0f;
	}

}