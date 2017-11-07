#include "MeshRenderer.h"

using namespace Device;
using namespace Rendering;
using namespace Rendering::Material;
using namespace Rendering::Renderer;
using namespace Rendering::Manager;
using namespace Rendering::Geometry;
using namespace Rendering::Buffer;
using namespace Rendering::Shader;
using namespace Rendering::RenderQueue;

void MeshRenderer::Initialize(DirectX& dx, ShaderManager& shaderMgr)
{
	_loader.Initialize(dx, shaderMgr);
}

void MeshRenderer::Destroy()
{
	_loader.Destroy();
}

template <class ShaderType>
void BindBasicCB(DirectX& dx, const Mesh& mesh, const ConstBuffer& camCB)
{
	ShaderType::BindConstBuffer(dx, ConstBufferBindIndex::Transform,	mesh.GetTransformCB());
	ShaderType::BindConstBuffer(dx, ConstBufferBindIndex::Camera,		camCB);
}

void MeshRenderer::RenderWithoutIASetVB(DirectX& dx, RenderParam param, DefaultRenderType renderType, Mesh& mesh) const
{	
	const IndexBuffer* indexBuffer = param.bufferMgr.GetPool<IndexBuffer>().Find(mesh.GetIBKey());
	assert(indexBuffer);

	indexBuffer->IASetBuffer(dx);

	uint key = _loader.MakeKey(mesh.GetBufferFlag(), renderType);

	const DefaultShaderLoader::Shaders& defaultShaders = _loader.Find(key);
	
	const auto& vs = defaultShaders.vs;
	const auto& gs = defaultShaders.gs;
	const auto& ps = defaultShaders.ps;
	
	auto BindSRV = [&dx, &gs, &ps](const auto& srObjects)
	{
		for(const auto& bind : srObjects.GetVector())
		{
			if (bind.useVS)
				VertexShader::BindShaderResourceView(dx, static_cast<TextureBindIndex>(bind.bindIndex), bind.resource.GetShaderResourceView());
			if (bind.useGS & gs.GetIsCanUse())
				GeometryShader::BindShaderResourceView(dx, static_cast<TextureBindIndex>(bind.bindIndex), bind.resource.GetShaderResourceView());
			if (bind.usePS & ps.GetIsCanUse())
				PixelShader::BindShaderResourceView(dx, static_cast<TextureBindIndex>(bind.bindIndex), bind.resource.GetShaderResourceView());
		}
	};

	// Binding
	{
		// Bind Shader To Context
		{
			vs.BindShaderToContext(dx);
			vs.BindInputLayoutToContext(dx);

			gs.BindShaderToContext(dx);
			ps.BindShaderToContext(dx);
		}

		BindBasicCB<VertexShader>(dx, mesh, param.camCB);
		if (ps.GetIsCanUse()) BindBasicCB<PixelShader>(dx, mesh, param.camCB);
		if (gs.GetIsCanUse()) BindBasicCB<GeometryShader>(dx, mesh, param.camCB);
	}

	for (MaterialID materialID : mesh.GetMaterialIDs())
	{
		auto material = param.materialMgr.Find<PhysicallyBasedMaterial>(materialID); assert(material);

		// ConstBuffers
		{
			const auto& cbs = material->GetConstBuffers();
			for(const auto& bindCB : cbs.GetVector())
			{
				if (bindCB.useVS)
					VertexShader::BindConstBuffer(dx, static_cast<ConstBufferBindIndex>(bindCB.bindIndex), bindCB.resource);
				if (bindCB.useGS & gs.GetIsCanUse())
					GeometryShader::BindConstBuffer(dx, static_cast<ConstBufferBindIndex>(bindCB.bindIndex), bindCB.resource);
				if (bindCB.usePS & ps.GetIsCanUse())
					PixelShader::BindConstBuffer(dx, static_cast<ConstBufferBindIndex>(bindCB.bindIndex), bindCB.resource);
			}
		}

		BindSRV(material->GetShaderResourceBuffers());
		BindSRV(material->GetTextures());

		dx.GetContext()->DrawIndexed(indexBuffer->GetIndexCount(), 0, 0);
	}

	VertexShader::UnBindShaderToContext(dx);
	PixelShader::UnBindShaderToContext(dx);
	GeometryShader::UnBindShaderToContext(dx);
}

void MeshRenderer::RenderTransparentMeshes(DirectX& dx, RenderParam param, DefaultRenderType renderType, TransparentMeshRenderQueue& meshes) const
{
	for (auto meshPtr : meshes)
	{
		const auto& vbPool					= param.bufferMgr.GetPool<VertexBuffer>();
		const VertexBuffer* vertexBuffer	= vbPool.Find(meshPtr->GetVBKey());

		vertexBuffer->IASetBuffer(dx);
		RenderWithoutIASetVB(dx, param, renderType, *meshPtr);
	}
}

void MeshRenderer::RenderOpaqueMeshes(DirectX& dx, RenderParam param, DefaultRenderType renderType, OpaqueMeshRenderQueue& meshes) const
{
	meshes.Iterate(
		[&dx, &param, renderType, this](Mesh* mesh)
		{
			const auto& vbPool		= param.bufferMgr.GetPool<VertexBuffer>();
			const VertexBuffer* vb	= vbPool.Find(mesh->GetVBKey());
			vb->IASetBuffer(dx);

			RenderWithoutIASetVB(dx, param, renderType, *mesh);
		}
	);
}