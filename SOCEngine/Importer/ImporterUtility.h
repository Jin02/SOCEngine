#pragma once

#include "Vector2.h"
#include "Vector3.h"
#include <utility>
#include <math.h>

namespace Importer
{
	class Utility
	{
	public:
		typedef Math::Vector2 TexCoord;

		static void CalculateTangentBinormal(Math::Vector3& outTangent, Math::Vector3& outBinormal, 
			const std::pair<const Math::Vector3&, const TexCoord&>& vertex1,
			const std::pair<const Math::Vector3&, const TexCoord&>& vertex2,
			const std::pair<const Math::Vector3&, const TexCoord&>& vertex3
			);

		static void CalculateNormal(Math::Vector3& outNormal, const Math::Vector3& tangent, const Math::Vector3& binormal);
	};
}