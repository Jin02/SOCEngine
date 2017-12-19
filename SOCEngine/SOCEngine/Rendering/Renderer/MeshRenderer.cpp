#include "MeshRenderer.h"
#include "AutoBinder.hpp"

using namespace Device;
using namespace Rendering;
using namespace Rendering::Material;
using namespace Rendering::Renderer;
using namespace Rendering::Manager;
using namespace Rendering::Geometry;
using namespace Rendering::Buffer;
using namespace Rendering::Shader;
using namespace Rendering::RenderQueue;

void MeshRenderer::RenderWithoutIASetVB(DirectX& dx, Param param, DefaultRenderType renderType, const Mesh& mesh)
{	
	const IndexBuffer* indexBuffer = param.bufferMgr.GetPool<IndexBuffer>().Find(mesh.GetIBKey()); assert(indexBuffer);
	indexBuffer->IASetBuffer(dx);

	const DefaultShaders::Shaders& defaultShaders = param.defaultShaders.Find(mesh.GetBufferFlag(), renderType);
	
	AutoBinderShaderToContext<VertexShader>		vsBinder(dx, defaultShaders.vs);
	AutoBinderShaderToContext<GeometryShader>	gsBinder(dx, defaultShaders.gs);
	AutoBinderShaderToContext<PixelShader>		psBinder(dx, defaultShaders.ps);

	AutoBinderInputLayoutToContext layoutBinder(dx, defaultShaders.vs);

	VertexShader::BindConstBuffer(dx, ConstBufferBindIndex::Transform, mesh.GetTransformCB());
	if (defaultShaders.ps.GetIsCanUse()) PixelShader::BindConstBuffer(dx, ConstBufferBindIndex::Transform, mesh.GetTransformCB());
	if (defaultShaders.gs.GetIsCanUse()) GeometryShader::BindConstBuffer(dx, ConstBufferBindIndex::Transform, mesh.GetTransformCB());

	dx.GetContext()->DrawIndexed(indexBuffer->GetIndexCount(), 0, 0);

	VertexShader::UnBindConstBuffer(dx, ConstBufferBindIndex::Transform);
	if (defaultShaders.ps.GetIsCanUse()) PixelShader::UnBindConstBuffer(dx, ConstBufferBindIndex::Transform);
	if (defaultShaders.gs.GetIsCanUse()) GeometryShader::UnBindConstBuffer(dx, ConstBufferBindIndex::Transform);
}