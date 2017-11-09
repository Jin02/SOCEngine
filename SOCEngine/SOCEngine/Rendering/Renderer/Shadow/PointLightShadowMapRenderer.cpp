#include "PointLightShadowMapRenderer.h"
#include "ShadowManager.h"

#include "VertexShader.h"

using namespace Rendering::Renderer::ShadowMap;
using namespace Rendering::Renderer;
using namespace Rendering::Manager;
using namespace Rendering::Shadow;
using namespace Rendering::Shader;
using namespace Rendering::Texture;
using namespace Rendering::Buffer;
using namespace Rendering::Geometry;
using namespace Rendering::RenderState;
using namespace Rendering::Material;
using namespace Device;
using namespace Core;

void PointLightShadowMapRenderer::Render(DirectX& dx, Param&& param, const TempRenderQueue& renderQ, const MeshRenderer::Param& meshParam, const CBType& shadowMapVPMatCB)
{
	float mapResolution	= static_cast<float>(param.atlasMapResolution);

	dx.SetDepthMapWithoutRenderTarget(param.shadowMap);
	dx.SetDepthStencilState(DepthState::Greater, 0);
	dx.SetPrimitiveTopology(PrimitiveTopology::TriangleList);
	dx.SetBlendState(BlendState::Opaque);

	param.shadowMap.Clear(dx, 0.0f, 0u);

	auto viewport = Rect<float>(mapResolution * float(param.shadowIndex), 0.0f, 
								mapResolution, mapResolution);

	PixelShader::BindSamplerState(dx, SamplerStateBindIndex::DefaultSamplerState, SamplerState::Linear);
	for (uint i = 0; i < 6; ++i)
	{
		viewport.y = float(i) * mapResolution;
		dx.SetViewport(viewport);

		VertexShader::BindConstBuffer(dx, ConstBufferBindIndex::OnlyPassViewProjMat, shadowMapVPMatCB[i]);
		_Render(dx, param.materialMgr, renderQ, meshParam);
		VertexShader::UnBindConstBuffer(dx, ConstBufferBindIndex::OnlyPassViewProjMat);
	}

	VertexShader::UnBindConstBuffer(dx, ConstBufferBindIndex::OnlyPassViewProjMat);
	dx.SetRasterizerState(RasterizerState::CWDefault);
}
