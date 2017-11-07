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
using namespace Rendering::RenderState;
using namespace Device;
using namespace Core;

void PointLightShadowMapRenderer::Render(
	DirectX& dx, Param&& param,
	TempRenderQueue& renderQ, const RenderManagerParam& mgrParam	)
{
	float mapResolution	= static_cast<float>(param.atlasMapResolution);

	dx.SetDepthMapWithoutRenderTarget(param.shadowMap);
	dx.SetDepthStencilState(DepthState::Greater, 0);

	param.shadowMap.Clear(dx, 0.0f, 0u);

	auto viewport = Rect<float>(mapResolution * float(param.shadowIndex), 0.0f, 
								mapResolution, mapResolution);

	for (uint i = 0; i < 6; ++i)
	{
		viewport.y = float(i) * mapResolution;
		dx.SetViewport(viewport);

		VertexShader::BindConstBuffer(dx, ConstBufferBindIndex::OnlyPass, param.shadowMapCB[i]);

		dx.SetRasterizerState(RasterizerState::CWDefault);
		mgrParam.meshRenderer.RenderOpaqueMeshes(dx, mgrParam.meshRenderParam, DefaultRenderType::Forward_OnlyDepth, renderQ.opaqueRenderQ);

		dx.SetRasterizerState(RasterizerState::CWDisableCulling);
		mgrParam.meshRenderer.RenderAlphaTestMeshes(dx,mgrParam.meshRenderParam, DefaultRenderType::Forward_AlphaTestWithDiffuse, renderQ.alphaTestRenderQ);
	}

	VertexShader::UnBindConstBuffer(dx, ConstBufferBindIndex::OnlyPass);
	dx.SetRasterizerState(RasterizerState::CWDefault);
}
