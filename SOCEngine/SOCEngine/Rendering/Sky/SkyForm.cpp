#include "SkyForm.h"
#include "ResourceManager.h"

#include "BasicGeometryGenerator.h"
#include "BindIndexInfo.h"

using namespace Rendering::Sky;
using namespace Rendering::Geometry;
using namespace Rendering::Manager;
using namespace Rendering::Buffer;
using namespace Rendering::Shader;
using namespace Rendering::Camera;
using namespace Resource;
using namespace Math;

SkyForm::SkyForm() : _mesh(nullptr)
{
}

SkyForm::~SkyForm()
{
	Destroy();
}

void SkyForm::Initialize(const Rendering::Material* skyMaterial)
{
	const ResourceManager* resMgr	= ResourceManager::SharedInstance();
	BufferManager* bufferMgr		= resMgr->GetBufferManager();

	BasicGeometryGenerator gen;
	uint flag = 0;	//uint(RenderManager::DefaultVertexInputTypeFlag::NORMAL) |
					//uint(RenderManager::DefaultVertexInputTypeFlag::UV0);

	_mesh = new Mesh;
	auto CreateMeshContent = [&](const Mesh::CreateFuncArguments& args)
	{
		_mesh->Initialize(args, false, false);

		MeshRenderer* renderer = _mesh->GetMeshRenderer();
		if (renderer->GetMaterials().empty() == false)
			renderer->DeleteMaterial(0);

		renderer->AddMaterial(skyMaterial);
	};

//	gen.CreateBox(CreateMeshContent, Vector3(5000, 5000, 5000), flag);
	gen.CreateSphere(CreateMeshContent, 1.0f, 30, 30, flag);
}

void SkyForm::Destroy()
{
	SAFE_DELETE(_mesh);
}

void SkyForm::_Render(const Device::DirectX* dx,
	const Rendering::Texture::RenderTexture*& renderTarget,
	const Rendering::Texture::DepthBuffer*& opaqueDepthBuffer)
{
	ID3D11DeviceContext* context = dx->GetContext();

	// Rendering Option
	{
		context->RSSetState(dx->GetRasterizerStateCWDisableCulling());
	
		ID3D11SamplerState* linearSampler = dx->GetSamplerStateLinear();
		context->PSSetSamplers(uint(SamplerStateBindIndex::DefaultSamplerState), 1, &linearSampler);
	
		context->OMSetDepthStencilState(dx->GetDepthStateGreaterAndDisableDepthWrite(), 0);

		ID3D11RenderTargetView* rtv = renderTarget->GetRenderTargetView();
		ID3D11DepthStencilView* dsv = opaqueDepthBuffer->GetDepthStencilView();
		context->OMSetRenderTargets(1, &rtv, dsv);
	
		float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		context->OMSetBlendState(dx->GetBlendStateOpaque(), blendFactor, 0xffffffff);
	}

	MeshFilter* filter = _mesh->GetMeshFilter();
	filter->GetIndexBuffer()->IASetBuffer(context);
	filter->GetVertexBuffer()->IASetBuffer(context);

	MeshRenderer* renderer = _mesh->GetMeshRenderer();
	const auto& materials = renderer->GetMaterials();
	for (const auto& material : materials)
	{
		const auto& customShader = material->GetCustomShader().shaderGroup;

		ASSERT_COND_MSG(customShader.ableRender(), "Error,  this shader is invalid")
		{
			const auto& cbs = material->GetConstBuffers();
			const auto& srbs = material->GetShaderResourceBuffers();
			const auto& texs = material->GetTextures();

			VertexShader* vs = customShader.vs;
			vs->BindInputLayoutToContext(context);
			vs->BindShaderToContext(context);
			vs->BindResourcesToContext(context, &cbs, &texs, &srbs);

			PixelShader* ps		= customShader.ps;
			ps->BindShaderToContext(context);
			ps->BindResourcesToContext(context, &cbs, &texs, &srbs);

			GeometryShader* gs	= customShader.gs;
			if (gs)
			{
				gs->BindShaderToContext(context);
				gs->BindResourcesToContext(context, &cbs, &texs, &srbs);
			}

			context->DrawIndexed(filter->GetIndexCount(), 0, 0);

			context->VSSetShader(nullptr, nullptr, 0);
			context->PSSetShader(nullptr, nullptr, 0);

			if (gs)
				context->GSSetShader(nullptr, nullptr, 0);
		}
	}
}