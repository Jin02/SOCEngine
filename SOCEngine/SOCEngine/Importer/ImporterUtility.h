#pragma once

#include "Common.h"
#include "Vector2.h"
#include "Vector3.h"
#include <utility>
#include <math.h>
#include <vector>
#include <array>

namespace Importer
{
	class ImporterUtility
	{
	public:
		static void CalculateTangent(Math::Vector3& outTangents, const std::array<Math::Vector3, 3>& vertex, const std::array<Math::Vector2, 3>& uvs);
	};
}