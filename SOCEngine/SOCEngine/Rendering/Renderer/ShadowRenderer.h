#pragma once

#include "ShadowManager.h"
#include "Rect.h"
#include "DepthMap.h"
#include "ShadowAtlasMap.h"

namespace Rendering
{
	namespace Renderer
	{
		class ShadowRenderer final
		{
		public:
			void Initialize(Device::DirectX& dx, uint dlMapResolution, uint slMapResolution, uint plMapResolution);
			void Destroy();

			struct ReSizeParam
			{
				uint mapResolution	= 128;
				uint capacity		= 8;

				ReSizeParam(uint resolution, uint _capacity)
					: mapResolution(resolution), capacity(_capacity) { }
			};

			template <typename ShadowType>
			void ReSizeShadowMap(Device::DirectX& dx, ReSizeParam param, bool useDestroy = true)
			{
				// Compute Param
				{
					auto Next2Squre = [](uint value) -> uint
					{
						return 1 << (uint)(ceil(log((uint)value) / log(2.0f)));
					};

					auto& atlasMap = GetShadowAtlasMap<ShadowType>();

					bool changedShadowMap =
						(atlasMap.GetResolution() != param.mapResolution) |
						(atlasMap.GetCapacity() < param.capacity);

					if (changedShadowMap == false) return;

					param.capacity = Next2Squre(param.capacity);
					param.mapResolution = Next2Squre(param.mapResolution);
				}

				// Resize Map
				{
					if (useDestroy)
						GetShadowAtlasMap<ShadowType>().Destroy();

					auto MakeMap = [&dx, &param](auto& depthMap, const Size<uint>& mapSize)
					{
						depthMap.Initialize(dx, mapSize, param.mapResolution, param.capacity);
					};

					Size<uint> mapSize = ComputeShadowAtlasMapSize<ShadowType>(param);
					MakeMap(GetShadowAtlasMap<ShadowType>(), mapSize);
				}
			}
			template <class ShadowType> auto& GetShadowAtlasMap()
			{
				return std::get<ShadowAtlasMap<ShadowType>>(_shadowAtlasMaps);
			}

		private:
			template <typename ShadowType>
			Size<uint> ComputeShadowAtlasMapSize(const ReSizeParam& param)
			{
				return Size<uint>(	param.mapResolution * param.capacity,
									param.mapResolution	);
			}
			template <>
			Size<uint> ComputeShadowAtlasMapSize<Shadow::PointLightShadow>(const ReSizeParam& param)
			{
				return Size<uint>(	param.mapResolution * param.capacity,
									param.mapResolution * 6	);
			}


		private:
			std::tuple<
				Shadow::ShadowAtlasMap<Shadow::DirectionalLightShadow>,
				Shadow::ShadowAtlasMap<Shadow::PointLightShadow>,
				Shadow::ShadowAtlasMap<Shadow::SpotLightShadow>
			> _shadowAtlasMaps;
		};
	}

	struct ShadowSystemParam
	{
		Manager::ShadowManager&		manager;
		Renderer::ShadowRenderer&	renderer;

		ShadowSystemParam(Manager::ShadowManager& _manager, Renderer::ShadowRenderer& _renderer)
			: manager(_manager), renderer(_renderer) { }
	};
}