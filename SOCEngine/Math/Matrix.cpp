#include "Matrix.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

namespace Math
{
	bool Matrix::operator != (const Matrix& mat) const
	{
	}

	bool Matrix::operator == (const Matrix& mat) const
	{
	}

	Matrix& Matrix::operator+= (const Matrix& mat)
	{
	}

	Matrix& Matrix::operator-= (const Matrix& mat)
	{
	}

	Matrix& Matrix::operator*= (const Matrix& mat)
	{

	}

	Matrix& Matrix::operator*= (float f)
	{
	}

	Matrix& Matrix::operator/= (float f)
	{
	}

	Matrix Matrix::operator+ (const Matrix& mat) const
	{

	}

	Matrix Matrix::operator- (const Matrix& mat) const
	{

	}

	Matrix Matrix::operator* (const Matrix& mat) const
	{

	}

	Matrix Matrix::operator* (float f) const
	{

	}

	Matrix Matrix::operator/ (float f) const
	{
	}

	const Vector2 Matrix::operator* (const Vector2& v) const
	{
	}

	const Vector3 Matrix::operator* (const Vector3& v) const
	{
	}

	const Vector4 Matrix::operator* (const Vector4& v) const
	{
	}
}