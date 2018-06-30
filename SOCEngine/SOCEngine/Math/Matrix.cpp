#include "Matrix.h"

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Quaternion.h"

#include <math.h>
#include <float.h>
#include <memory>

using namespace Math;

Matrix::Matrix(	float __11, float __12, float __13, float __14,
				float __21, float __22, float __23, float __24,
				float __31, float __32, float __33, float __34,
				float __41, float __42, float __43, float __44	)
	: 	_11(__11), _12(__12), _13(__13), _14(__14),
		_21(__21), _22(__22), _23(__23), _24(__24),
		_31(__31), _32(__32), _33(__33), _34(__34),
		_41(__41), _42(__42), _43(__43), _44(__44)
{
}

bool Matrix::operator != (const Matrix& mat) const
{
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
		{
			float f = abs(_m[i][j] - mat._m[i][j]);

			if (f > FLT_EPSILON)
				return true;
		}

	return false;
}

bool Matrix::operator == (const Matrix& mat) const
{
	return !(operator!=(mat));
}

Matrix& Matrix::operator+= (const Matrix& mat)
{
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			_m[i][j] += mat._m[i][j];

	return *this;
}

Matrix& Matrix::operator-= (const Matrix& mat)
{
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			_m[i][j] -= mat._m[i][j];

	return *this;
}

Matrix& Matrix::operator*= (const Matrix& mat)
{
	(*this) = Multiply((*this), mat);
	return (*this);
}

Matrix& Matrix::operator*= (float f)
{
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
			_m[i][j] *= f;

	return (*this);
}

Matrix& Matrix::operator/= (float f)
{
	return operator*=(1.0f / f);
}

const Matrix Matrix::operator+ (const Matrix& mat) const
{
	Matrix res(*this);

	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
			res._m[i][j] += mat._m[i][j];

	return res;
}

const Matrix Matrix::operator- (const Matrix& mat) const
{
	Matrix res(*this);

	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
			res._m[i][j] -= mat._m[i][j];

	return res;
}

const Matrix Matrix::operator* (const Matrix& mat)
{
	return Multiply((*this), mat);
}

const Matrix Matrix::operator* (float f) const
{
	Matrix res(*this);

	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
			res._m[i][j] *= f;

	return res;
}

const Matrix Matrix::operator* (const Matrix& mat) const
{
	return 	Matrix::Multiply((*this), mat);
}

const Matrix Matrix::operator/ (float f) const
{
	Matrix res(*this);

	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
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
	Vector4 res(
		v.x * _m[0][0] + v.y * _m[1][0] + v.z * _m[2][0] + v.w * _m[3][0],
		v.x * _m[0][1] + v.y * _m[1][1] + v.z * _m[2][1] + v.w * _m[3][1],
		v.x * _m[0][2] + v.y * _m[1][2] + v.z * _m[2][2] + v.w * _m[3][2],
		v.x * _m[0][3] + v.y * _m[1][3] + v.z * _m[2][3] + v.w * _m[3][3]
	);

	return res;
}

Matrix Matrix::Transpose(const Matrix& mat)
{
	return Matrix (	mat._11, mat._21, mat._31, mat._41,
					mat._12, mat._22, mat._32, mat._42,
					mat._13, mat._23, mat._33, mat._43,
					mat._14, mat._24, mat._34, mat._44	);
}

void Matrix::Set(	float _11, float _12, float _13, float _14,
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

Matrix Matrix::Multiply(const Matrix& lhs, const Matrix& rhs)
{
	Matrix res;
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			float f = 0.0f;
			for (int k = 0; k < 4; ++k)
			{
				f += lhs._m[i][k] * rhs._m[k][j];
			}

			res._m[i][j] = f;
		}
	}

	return res;
}

Matrix Matrix::Identity()
{
	return Matrix(	1.0f, 0.0f, 0.0f, 0.0f,
					0.0f, 1.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 1.0f, 0.0f,
					0.0f, 0.0f, 0.0f, 1.0f	);
}

Matrix Matrix::Inverse(const Matrix& mat)
{
	Matrix out;
	Matrix src = Matrix::Transpose(mat);

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
	out._m[0][0] = tmpCofactors[0] * src._m[1][1] + tmpCofactors[3] * src._m[1][2] + tmpCofactors[4] * src._m[1][3];
	out._m[0][0] -= tmpCofactors[1] * src._m[1][1] + tmpCofactors[2] * src._m[1][2] + tmpCofactors[5] * src._m[1][3];
	out._m[0][1] = tmpCofactors[1] * src._m[1][0] + tmpCofactors[6] * src._m[1][2] + tmpCofactors[9] * src._m[1][3];
	out._m[0][1] -= tmpCofactors[0] * src._m[1][0] + tmpCofactors[7] * src._m[1][2] + tmpCofactors[8] * src._m[1][3];
	out._m[0][2] = tmpCofactors[2] * src._m[1][0] + tmpCofactors[7] * src._m[1][1] + tmpCofactors[10] * src._m[1][3];
	out._m[0][2] -= tmpCofactors[3] * src._m[1][0] + tmpCofactors[6] * src._m[1][1] + tmpCofactors[11] * src._m[1][3];
	out._m[0][3] = tmpCofactors[5] * src._m[1][0] + tmpCofactors[8] * src._m[1][1] + tmpCofactors[11] * src._m[1][2];
	out._m[0][3] -= tmpCofactors[4] * src._m[1][0] + tmpCofactors[9] * src._m[1][1] + tmpCofactors[10] * src._m[1][2];
	out._m[1][0] = tmpCofactors[1] * src._m[0][1] + tmpCofactors[2] * src._m[0][2] + tmpCofactors[5] * src._m[0][3];
	out._m[1][0] -= tmpCofactors[0] * src._m[0][1] + tmpCofactors[3] * src._m[0][2] + tmpCofactors[4] * src._m[0][3];
	out._m[1][1] = tmpCofactors[0] * src._m[0][0] + tmpCofactors[7] * src._m[0][2] + tmpCofactors[8] * src._m[0][3];
	out._m[1][1] -= tmpCofactors[1] * src._m[0][0] + tmpCofactors[6] * src._m[0][2] + tmpCofactors[9] * src._m[0][3];
	out._m[1][2] = tmpCofactors[3] * src._m[0][0] + tmpCofactors[6] * src._m[0][1] + tmpCofactors[11] * src._m[0][3];
	out._m[1][2] -= tmpCofactors[2] * src._m[0][0] + tmpCofactors[7] * src._m[0][1] + tmpCofactors[10] * src._m[0][3];
	out._m[1][3] = tmpCofactors[4] * src._m[0][0] + tmpCofactors[9] * src._m[0][1] + tmpCofactors[10] * src._m[0][2];
	out._m[1][3] -= tmpCofactors[5] * src._m[0][0] + tmpCofactors[8] * src._m[0][1] + tmpCofactors[11] * src._m[0][2];

	/* calculate pairs for second 8 elements (cofactors) */
	tmpCofactors[0] = src._m[0][2] * src._m[1][3];
	tmpCofactors[1] = src._m[0][3] * src._m[1][2];
	tmpCofactors[2] = src._m[0][1] * src._m[1][3];
	tmpCofactors[3] = src._m[0][3] * src._m[1][1];
	tmpCofactors[4] = src._m[0][1] * src._m[1][2];
	tmpCofactors[5] = src._m[0][2] * src._m[1][1];
	tmpCofactors[6] = src._m[0][0] * src._m[1][3];
	tmpCofactors[7] = src._m[0][3] * src._m[1][0];
	tmpCofactors[8] = src._m[0][0] * src._m[1][2];
	tmpCofactors[9] = src._m[0][2] * src._m[1][0];
	tmpCofactors[10] = src._m[0][0] * src._m[1][1];
	tmpCofactors[11] = src._m[0][1] * src._m[1][0];

	/* calculate second 8 elements (cofactors) */
	out._m[2][0] = tmpCofactors[0] * src._m[3][1] + tmpCofactors[3] * src._m[3][2] + tmpCofactors[4] * src._m[3][3];
	out._m[2][0] -= tmpCofactors[1] * src._m[3][1] + tmpCofactors[2] * src._m[3][2] + tmpCofactors[5] * src._m[3][3];
	out._m[2][1] = tmpCofactors[1] * src._m[3][0] + tmpCofactors[6] * src._m[3][2] + tmpCofactors[9] * src._m[3][3];
	out._m[2][1] -= tmpCofactors[0] * src._m[3][0] + tmpCofactors[7] * src._m[3][2] + tmpCofactors[8] * src._m[3][3];
	out._m[2][2] = tmpCofactors[2] * src._m[3][0] + tmpCofactors[7] * src._m[3][1] + tmpCofactors[10] * src._m[3][3];
	out._m[2][2] -= tmpCofactors[3] * src._m[3][0] + tmpCofactors[6] * src._m[3][1] + tmpCofactors[11] * src._m[3][3];
	out._m[2][3] = tmpCofactors[5] * src._m[3][0] + tmpCofactors[8] * src._m[3][1] + tmpCofactors[11] * src._m[3][2];
	out._m[2][3] -= tmpCofactors[4] * src._m[3][0] + tmpCofactors[9] * src._m[3][1] + tmpCofactors[10] * src._m[3][2];
	out._m[3][0] = tmpCofactors[2] * src._m[2][2] + tmpCofactors[5] * src._m[2][3] + tmpCofactors[1] * src._m[2][1];
	out._m[3][0] -= tmpCofactors[4] * src._m[2][3] + tmpCofactors[0] * src._m[2][1] + tmpCofactors[3] * src._m[2][2];
	out._m[3][1] = tmpCofactors[8] * src._m[2][3] + tmpCofactors[0] * src._m[2][0] + tmpCofactors[7] * src._m[2][2];
	out._m[3][1] -= tmpCofactors[6] * src._m[2][2] + tmpCofactors[9] * src._m[2][3] + tmpCofactors[1] * src._m[2][0];
	out._m[3][2] = tmpCofactors[6] * src._m[2][1] + tmpCofactors[11] * src._m[2][3] + tmpCofactors[3] * src._m[2][0];
	out._m[3][2] -= tmpCofactors[10] * src._m[2][3] + tmpCofactors[2] * src._m[2][0] + tmpCofactors[7] * src._m[2][1];
	out._m[3][3] = tmpCofactors[10] * src._m[2][2] + tmpCofactors[4] * src._m[2][0] + tmpCofactors[9] * src._m[2][1];
	out._m[3][3] -= tmpCofactors[8] * src._m[2][1] + tmpCofactors[11] * src._m[2][2] + tmpCofactors[5] * src._m[2][0];

	/* calculate determinant */
	float det = src._m[0][0] * out._m[0][0] + src._m[0][1] * out._m[0][1] + src._m[0][2] * out._m[0][2] + src._m[0][3] * out._m[0][3];

	/* calculate matrix inverse */
	det = 1 / det;
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; ++j)
			out._m[i][j] *= det;

	return out;
}

Matrix Matrix::RotateUsingQuaternion(const Quaternion& q)
{	
	float qy2 = q.y * q.y;
	float qx2 = q.x * q.x;
	float qz2 = q.z * q.z;	
	
	return Matrix(
		1.0f - 2.0f * (qy2 + qz2),
		2.0f * (q.x * q.y - q.z * q.w),
		2.0f * (q.x * q.z + q.y * q.w),
		0.0f,

		2.0f * (q.x * q.y + q.z * q.w),		
		1.0f - 2.0f * (qx2 + qz2),
		2.0f * (q.y * q.z - q.x * q.w),
		0.0f,
				
		2.0f * (q.x * q.z - q.y * q.w),
		2.0f * (q.y * q.z + q.x * q.w),
		1.0f - 2.0f * (qx2 + qy2),
		0.0f,
		
		0.0f,
		0.0f,
		0.0f,
		1.0f	);
}

Matrix Matrix::PerspectiveFovLH(float aspect, float fovy, float zn, float zf)
{
	float yScale = 1.0f / tanf(fovy / 2.0f);
	float xScale = yScale / aspect;

	return Matrix(	xScale,		0.0f,		0.0f,					0.0f,
					0.0f,		yScale,		0.0f,					0.0f,
					0.0f,		0.0f,		zf / (zf - zn),			1.0f,
					0.0f,		0.0f,		-zn * zf / (zf - zn),	0.0f	);
}

Matrix Matrix::OrthoLH(float w, float h, float zn, float zf)
{
	return Matrix(	2.0f / w,	0.0f,		0.0f,				0.0f,
					0.0f,		2.0f / h,	0.0f,				0.0f,
					0.0f,		0.0f,		1.0f / (zf- zn),	0.0f,
					0.0f,		0.0f,		zn / (zn - zf),		1.0f	);
}

Matrix Matrix::RotateUsingAxis(const Vector3& v, float angle)
{
	float sangle	= sinf(angle);
	float cangle	= cosf(angle);
	float cdiff		= 1.0f - cangle;
	Vector3 nv		= Vector3::Normalize(v);

	return Matrix(	cdiff * nv.x * nv.x + cangle,			cdiff * nv.y * nv.x + sangle * nv.z,	cdiff * nv.z * nv.x - sangle * nv.y,	0.0f,
					cdiff * nv.x * nv.y - sangle * nv.z,	cdiff * nv.y * nv.y + cangle,			cdiff * nv.z * nv.y + sangle * nv.x,	0.0f,
					cdiff * nv.x * nv.z + sangle * nv.y,	cdiff * nv.y * nv.z - sangle * nv.x,	cdiff * nv.z * nv.z + cangle,			0.0f,
					0.0f,									0.0f,									0.0f,									1.0f	);
}

Matrix Matrix::ComputeViewMatrix(const Matrix& worldMatrix)
{
	Vector3 right		= Vector3(worldMatrix._11, worldMatrix._12, worldMatrix._13);
	Vector3 up			= Vector3(worldMatrix._21, worldMatrix._22, worldMatrix._23);
	Vector3 forward		= Vector3(worldMatrix._31, worldMatrix._32, worldMatrix._33);
	Vector3 worldPos	= Vector3(worldMatrix._41, worldMatrix._42, worldMatrix._43);

	Vector3 p			= Vector3(	-Vector3::Dot(right, worldPos),
									-Vector3::Dot(up, worldPos),
									-Vector3::Dot(forward, worldPos)	);

	return Matrix(
		right.x, up.x, forward.x, 0.0f,
		right.y, up.y, forward.y, 0.0f,
		right.z, up.z, forward.z, 0.0f,
		p.x, p.y, p.z, 1.0f
	);
}

Matrix Matrix::ComputeViewportMatrix(const Rect<uint>& rect)
{
	return Matrix(	float(rect.size.w) / 2.0f,			0.0f,								0.0f,	0.0f,
					0.0f,								-float(rect.size.h) / 2.0f,			0.0f,	0.0f,
					0.0f,								0.0f,								1.0f,	0.0f,
					float(rect.x + rect.size.w) / 2.0f,	float(rect.y + rect.size.h) / 2.0f,	0.0f,	1.0f	);
}

Matrix Matrix::ComputeInvViewportMatrix(const Rect<uint>& rect)
{
	Matrix viewportMat = ComputeViewportMatrix(rect);
	return Matrix::Inverse(viewportMat);
}

inline Matrix Math::Matrix::MakeRotationMatrix(const Vector3& right, const Vector3& up, const Vector3& forward)
{
	return Matrix(	right.x,	right.y,	right.z,	0.0f,
					up.x,		up.y,		up.z,		0.0f,
					forward.x,	forward.y,	forward.z,	0.0f,
					0.0f,		0.0f,		0.0f,		1.0f	);
}

Matrix Math::Matrix::LookAtDir(const Vector3& targetDir, const Vector3 * upVec)
{
	Vector3 worldUp	= upVec ? *upVec : Vector3::Up();

	Vector3 forward	= targetDir.Normalized();
	Vector3 right	= Vector3::Cross(worldUp, forward);
	Vector3 up		= Vector3::Cross(forward, right);

	return Matrix::MakeRotationMatrix(right, up, forward);
}

Matrix Matrix::ComputeViewProjMatrix(const Vector3& eyePos, const Vector3& forward, const Vector3& up, const Matrix& projMat)
{
	Matrix view = Matrix::LookAtDir(forward, &up);

	view._41 = eyePos.x;
	view._42 = eyePos.y;
	view._43 = eyePos.z;
	view._44 = 1.0f;

	view = Matrix::ComputeViewMatrix(view);
	
	return view * projMat;
}
