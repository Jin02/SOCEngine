#pragma once

#include "Common.h"
#include "Vector2.h"
#include "Vector3.h"
#include <utility>
#include <math.h>
#include <vector>

namespace Importer
{
	class Utility
	{
	public:
		static void CalculateTangentBinormal(std::vector<Math::Vector3>& outTangents, std::vector<Math::Vector3>& outBinormal, const Math::Vector3* vertices, unsigned int vertexCount, const Math::Vector3* normals, const Math::Vector2* texcoords, const ENGINE_INDEX_TYPE* indices, unsigned int indexCount);		
	};
}