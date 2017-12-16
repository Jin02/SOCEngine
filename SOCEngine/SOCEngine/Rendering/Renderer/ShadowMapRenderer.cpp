#include "ShadowMapRenderer.h"
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
using namespace Rendering::Material;
using namespace Rendering::Geometry;
using namespace Device;
using namespace Core;

void ShadowMapRenderer::_Render(DirectX& dx, const MaterialManager& materialMgr, const TempRenderQueue& renderQ, const MeshRenderer::Param& meshParam)
{
	dx.SetRasterizerState(RasterizerState::CWDefault);
	MeshRenderer::RenderOpaqueMeshes(dx, meshParam, DefaultRenderType::Forward_OnlyDepth, renderQ.opaqueRenderQ, [](const Mesh* mesh){ }, [](){});

	dx.SetRasterizerState(RasterizerState::CWDisableCulling);
	MeshRenderer::RenderAlphaTestMeshes(dx, meshParam, DefaultRenderType::Forward_AlphaTestWithDiffuse, renderQ.alphaTestRenderQ,
		[&dx, &materialMgr](const Mesh* mesh)
		{
			auto material	= materialMgr.Find<PhysicallyBasedMaterial>(mesh->GetPBRMaterialID()); assert(material);
	
			auto diffuseMap	= material->GetTextures().Find(PhysicallyBasedMaterial::GetDiffuseMapKey()); assert(diffuseMap);
			PixelShader::BindShaderResourceView(dx, TextureBindIndex::DiffuseMap, diffuseMap->resource.GetShaderResourceView());
	
			auto opacityMap = material->GetTextures().Find(PhysicallyBasedMaterial::GetOpacityMapKey());
			if (opacityMap)
				PixelShader::BindShaderResourceView(dx, TextureBindIndex::OpacityMap, opacityMap->resource.GetShaderResourceView());
		},
		[&dx]()
		{
			PixelShader::UnBindShaderResourceView(dx, TextureBindIndex::OpacityMap);
			PixelShader::UnBindShaderResourceView(dx, TextureBindIndex::DiffuseMap);
		}
	);
}

void ShadowMapRenderer::Render(DirectX& dx, Param&& param, const TempRenderQueue& renderQ, const MeshRenderer::Param& meshParam, const ConstBuffer& shadowMapVPMatCB)
{
	float mapResolution	= static_cast<float>(param.atlasMapResolution);
	auto viewport		= Rect<float>(mapResolution * float(param.shadowIndex), 0.0f, mapResolution, mapResolution);

	dx.SetViewport(viewport);
	dx.SetDepthMapWithoutRenderTarget(param.shadowMap);
	dx.SetDepthStencilState(DepthState::Greater, 0);
	dx.SetPrimitiveTopology(PrimitiveTopology::TriangleList);
	dx.SetBlendState(BlendState::Opaque);

	param.shadowMap.Clear(dx, 0.0f, 0u);

	PixelShader::BindSamplerState(dx, SamplerStateBindIndex::DefaultSamplerState, SamplerState::Linear);
	VertexShader::BindConstBuffer(dx, ConstBufferBindIndex::OnlyPassViewProjMat, shadowMapVPMatCB);
	_Render(dx, param.materialMgr, renderQ, meshParam);
	VertexShader::UnBindConstBuffer(dx, ConstBufferBindIndex::OnlyPassViewProjMat);

	dx.SetRasterizerState(RasterizerState::CWDefault);
}
