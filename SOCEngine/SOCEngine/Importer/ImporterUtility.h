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
		static void CalculateTangentBinormal(Math::Vector3& outTangents, Math::Vector3& outBinormal,
			const std::array<const Math::Vector3*, 3>& vertex,
			const std::array<const Math::Vector2*, 3>& uvs);
		static void CalculateNormal(Math::Vector3& outNormal, const Math::Vector3& binormal, const Math::Vector3& tangent);	
	};
}