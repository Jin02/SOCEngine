#pragma once

#include "Common.h"
#include "ShadowAtlasMap.hpp"
#include "MeshRenderer.h"

#include "RendererCommon.h"

namespace Rendering
{
	namespace Manager
	{
		class ShadowManager;
	}

	namespace Renderer
	{
		namespace ShadowMap
		{
			class ShadowMapRenderer
			{
			public:
				struct Param
				{
					const Manager::MaterialManager&	materialMgr;
					Texture::DepthMap&				shadowMap;
					uint							shadowIndex;
					uint							atlasMapResolution;

					Param(const Manager::MaterialManager& _materialMgr, Texture::DepthMap& _shadowMap, uint _shadowIndex, uint _atlasMapResolution)
						: materialMgr(_materialMgr), shadowMap(_shadowMap), shadowIndex(_shadowIndex), atlasMapResolution(_atlasMapResolution) { }
				};

			public:
				static void Render(Device::DirectX& dx, Param&& param, const TempRenderQueue& renderQ, const MeshRenderer::Param& meshParam, const Buffer::ConstBuffer& shadowMapVPMatCB);

			protected:
				static void _Render(Device::DirectX& dx, const Manager::MaterialManager& materialMgr, const TempRenderQueue& renderQ, const MeshRenderer::Param& meshParam);
			};
		}
	}
}