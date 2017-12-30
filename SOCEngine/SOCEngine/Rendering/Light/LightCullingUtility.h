#pragma once

#include "Common.h"
#include "Rect.h"
#include "GlobalDefine.h"

namespace Rendering
{
	namespace Light
	{
		class CullingUtility final
		{
		public:
			DISALLOW_COPY_CONSTRUCTOR(CullingUtility);

		public:
			static const Size<uint> ComputeThreadGroupSize(const Size<uint>& size)
			{
				return Size<uint>(
					static_cast<uint>((size.w + LIGHT_CULLING_TILE_RES - 1) / static_cast<float>(LIGHT_CULLING_TILE_RES)),
					static_cast<uint>((size.h + LIGHT_CULLING_TILE_RES - 1) / static_cast<float>(LIGHT_CULLING_TILE_RES))	
					);
			}

			static uint CalcMaxNumLightsInTile(const Size<uint>& size)
			{
				const uint key = LIGHT_CULLING_TILE_RES;
				return (LIGHT_CULLING_LIGHT_MAX_COUNT_IN_TILE - (key * (size.h / 120)));
			}

			static const Size<uint> CalcThreadGroupSize(const Size<uint>& size)
			{
				auto CalcThreadLength = [](unsigned int size)
				{
					return (unsigned int)((size + LIGHT_CULLING_TILE_RES - 1) / (float)LIGHT_CULLING_TILE_RES);
				};

				unsigned int width = CalcThreadLength(size.w);
				unsigned int height = CalcThreadLength(size.h);

				return Size<uint>(width, height);
			}

		};
	}
}