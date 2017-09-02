#include "MeshRenderer.h"

using namespace Device;
using namespace Rendering;
using namespace Rendering::Material;
using namespace Rendering::Renderer;
using namespace Rendering::Manager;
using namespace Rendering::Geometry;
using namespace Rendering::Buffer;
using namespace Rendering::Shader;

void MeshRenderer::Initialize(DirectX& dx, ShaderManager& shaderMgr)
{
	_loader.Initialize(dx, shaderMgr);
}

void MeshRenderer::Destroy()
{
	_loader.Destroy();
}

template <class ShaderType>
void BindBasicCB(DirectX& dx, Mesh& mesh, ConstBuffer& camCB)
{
	ShaderType::BindConstBuffer(dx, ConstBufferBindIndex::Transform, mesh.GetTransformCB());
	ShaderType::BindConstBuffer(dx, ConstBufferBindIndex::Camera, camCB);
}

/*
void MeshRenderer::RenderWithoutIASetVB(RenderParam param, Mesh& mesh)
{	
	//auto& indexBuffer = mesh.GetIndexBuffer();
	//indexBuffer.IASetBuffer(param.dx);

	//uint key = _loader.MakeKey(mesh.GetBufferFlag(), param.renderType);

	//DefaultShaderLoader::Shaders& defaultShaders = _loader.Find(key);
	//auto& vs = defaultShaders.vs; auto& gs = defaultShaders.gs; auto& ps = defaultShaders.ps;
	//auto BindSRV = [&dx = param.dx, &gs, &ps](auto& srObjects)
	//{
	//	uint size = srObjects.GetSize();
	//	for (uint i = 0; i < size; ++i)
	//	{
	//		auto& bind = srObjects.Get(i);
	//		if (bind.useVS)
	//			VertexShader::BindShaderResourceView(dx, static_cast<TextureBindIndex>(bind.bindIndex), bind.resource.GetShaderResourceView());
	//		if (bind.useGS & gs.GetIsCanUse())
	//			GeometryShader::BindShaderResourceView(dx, static_cast<TextureBindIndex>(bind.bindIndex), bind.resource.GetShaderResourceView());
	//		if (bind.usePS & ps.GetIsCanUse())
	//			PixelShader::BindShaderResourceView(dx, static_cast<TextureBindIndex>(bind.bindIndex), bind.resource.GetShaderResourceView());
	//	}
	//};

	//// Binding
	//{
	//	// Bind Shader To Context
	//	{
	//		vs.BindShaderToContext(param.dx);
	//		vs.BindInputLayoutToContext(param.dx);

	//		gs.BindShaderToContext(param.dx);
	//		ps.BindShaderToContext(param.dx);
	//	}

	//	BindBasicCB<VertexShader>(param.dx, mesh, param.camCB);
	//	if (ps.GetIsCanUse()) BindBasicCB<PixelShader>(param.dx, mesh, param.camCB);
	//	if (gs.GetIsCanUse()) BindBasicCB<GeometryShader>(param.dx, mesh, param.camCB);
	//}

	//for (MaterialID materialID : mesh.GetMaterialIDs())
	//{
	//	auto material = param.materialMgr.Find<PhysicallyBasedMaterial>(materialID); assert(material);

	//	// ConstBuffers
	//	{
	//		auto& cbs = material->GetConstBuffers();
	//		uint size = cbs.GetSize();
	//		for (uint i = 0; i < size; ++i)
	//		{
	//			auto& bindCB = cbs.Get(i);
	//			if (bindCB.useVS)
	//				VertexShader::BindConstBuffer(param.dx, static_cast<ConstBufferBindIndex>(bindCB.bindIndex), bindCB.resource);
	//			if (bindCB.useGS & gs.GetIsCanUse())
	//				GeometryShader::BindConstBuffer(param.dx, static_cast<ConstBufferBindIndex>(bindCB.bindIndex), bindCB.resource);
	//			if (bindCB.usePS & ps.GetIsCanUse())
	//				PixelShader::BindConstBuffer(param.dx, static_cast<ConstBufferBindIndex>(bindCB.bindIndex), bindCB.resource);
	//		}
	//	}

	//	BindSRV(material->GetShaderResourceBuffers());
	//	BindSRV(material->GetTextures());

	//	param.dx.GetContext()->DrawIndexed(mesh.GetIndexBuffer().GetIndexCount(), 0, 0);
	//}

	//param.dx.GetContext()->VSSetShader(nullptr, nullptr, 0);
	//param.dx.GetContext()->GSSetShader(nullptr, nullptr, 0);
	//param.dx.GetContext()->PSSetShader(nullptr, nullptr, 0);
}

void MeshRenderer::RenderUsingSortedMeshVectorByVB(RenderParam param, MeshPool& meshes)
{
	uint meshesCount = meshes.GetSize();
	for (uint i = 0; i < meshesCount; ++i)
	{
		Mesh& mesh = meshes.Get(i);

	}
}

*/