#pragma once

#include "PointLightShadowMapCBPool.h"
#include "ShadowMapRenderer.h"

namespace Rendering
{
	namespace Renderer
	{
		namespace ShadowMap
		{
			class PointLightShadowMapRenderer final : private ShadowMapRenderer 
			{
			public:
				using ShadowMapRenderer::Param;

				using CBType = Shadow::Buffer::PointLightShadowMapCBPool::ConstBufferType;
				static void Render(Device::DirectX& dx, Param&& param, const TempRenderQueue& renderQ, const MeshRenderer::Param& meshParam, const CBType& shadowMapVPMatCB);
			};
		}
	}
}