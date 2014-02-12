#include "Common.h"

namespace Math
{
	float Common::Rad2Deg()
	{
		return 180.0f / PI;
	}

	float Common::Deg2Rad()
	{
		return PI / 180.0f;
	}
}