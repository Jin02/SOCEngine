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

	void Common::EulerNormalize(Vector3& out, const Vector3& eulerAngles)
	{
		int x = (int)(eulerAngles.x / 360.0f);
		int y = (int)(eulerAngles.y / 360.0f);
		int z = (int)(eulerAngles.z / 360.0f);

		out.x = eulerAngles.x - x * 360.0f + (int)(eulerAngles.x < 0) * 360.0f;
		out.y = eulerAngles.y - y * 360.0f + (int)(eulerAngles.y < 0) * 360.0f;
		out.z = eulerAngles.z - z * 360.0f + (int)(eulerAngles.z < 0) * 360.0f;
	}

}