#pragma once

#include "ShadowManager.h"
#include "ShadowAtlasMap.h"

namespace Rendering
{
	namespace Renderer
	{
		template <class ShadowType>
		class ShadowAtlasMapObject : public Shadow::ShadowAtlasMap {};

		class ShadowRenderer final
		{
		public:
			void Initialize(Device::DirectX& dx);
			void Destroy();

			template <class ShadowType>
			auto& GetShadowAtlasMap()
			{
				return std::get<ShadowAtlasMapObject<ShadowType>>(_shadowAtlasMaps);
			}

		private:
			std::tuple<
				ShadowAtlasMapObject<Shadow::DirectionalLightShadow>,
				ShadowAtlasMapObject<Shadow::PointLightShadow>,
				ShadowAtlasMapObject<Shadow::SpotLightShadow>
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