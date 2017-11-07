#pragma once

#include "Common.h"
#include "ShadowAtlasMap.h"
#include "MeshRenderer.h"

#include "ShadowRendererCommon.h"

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
			class ShadowMapRenderer final
			{
			public:
				struct Param
				{
					const Buffer::ConstBuffer&	shadowMapCB;
					Texture::DepthMap&			shadowMap;
					uint						shadowIndex;
					uint						atlasMapResolution;

					Param(const Buffer::ConstBuffer& _shadowMapCB, Texture::DepthMap& _shadowMap, uint _shadowIndex, uint _atlasMapResolution)
						: shadowMapCB(_shadowMapCB), shadowMap(_shadowMap), shadowIndex(_shadowIndex), atlasMapResolution(_atlasMapResolution) { }
				};

			public:
				static void Render(
					Device::DirectX& dx, Param&& param,
					TempRenderQueue& renderQ, const RenderManagerParam& mgrParam );
			};
		}
	}
}