#pragma once

#include "Common.h"
#include "ShadowAtlasMap.h"
#include "MeshRenderer.h"

#include "ShadowRendererCommon.h"
#include "PointLightShadowMapCBPool.h"

namespace Rendering
{
	namespace Renderer
	{
		namespace ShadowMap
		{
			class PointLightShadowMapRenderer final
			{
			public:
				struct Param
				{
					using CBType = Shadow::Buffer::PointLightShadowMapCBPool::ConstBufferType;

					const CBType&				shadowMapCB;
					Texture::DepthMap&			shadowMap;
					uint						shadowIndex;
					uint						atlasMapResolution;

					Param(const CBType& _shadowMapCB, Texture::DepthMap& _shadowMap, uint _shadowIndex, uint _atlasMapResolution)
						: shadowMapCB(_shadowMapCB), shadowMap(_shadowMap), shadowIndex(_shadowIndex), atlasMapResolution(_atlasMapResolution) { }
				};

			public:
				static void Render(
					Device::DirectX& dx, Param&& param,
					TempRenderQueue& renderQ, const RenderManagerParam& mgrParam
				);
			};
		}
	}
}