#include "MathCommon.h"
#include "Vector3.h"

namespace Math
{
	float Common::Rad2Deg(float a)
	{
		return 180.0f / PI * a;
	}

	float Common::Deg2Rad(float a)
	{
		return PI / 180.0f * a;
	}

	void Common::EulerNormalize(Vector3& out, const Vector3& eulerAngels)
	{
		int x = (int)(eulerAngels.x / 360.0f);
		int y = (int)(eulerAngels.y / 360.0f);
		int z = (int)(eulerAngels.z / 360.0f);

		out.x = eulerAngels.x - x * 360.0f + (int)(eulerAngels.x < 0) * 360.0f;
		out.y = eulerAngels.y - y * 360.0f + (int)(eulerAngels.y < 0) * 360.0f;
		out.z = eulerAngels.z - z * 360.0f + (int)(eulerAngels.z < 0) * 360.0f;
	}

}