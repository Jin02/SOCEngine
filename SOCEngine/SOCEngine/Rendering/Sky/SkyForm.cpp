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
using namespace Rendering::Texture;
using namespace Resource;
using namespace Math;

SkyForm::SkyForm(Type type) : _meshFilter(nullptr),
	_isSkyLightOn(true), _isDynamicSkyLight(false), _blendFraction(0.0f), _maxMipCount(10.0f), _type(type)
{
}

SkyForm::~SkyForm()
{
	Destroy();
}

void SkyForm::Initialize()
{
	const ResourceManager* resMgr	= ResourceManager::SharedInstance();
	BufferManager* bufferMgr		= resMgr->GetBufferManager();

	_meshFilter = new MeshFilter;
	auto CreateMeshContent = [&](const Mesh::CreateFuncArguments& args)
	{
		_meshFilter->Initialize(args);
	};

	BasicGeometryGenerator gen;
	gen.CreateSphere(CreateMeshContent, 1.0f, 64, 64, 0);
}

void SkyForm::Destroy()
{
	SAFE_DELETE(_meshFilter);
}

void SkyForm::_Render(const Device::DirectX* dx, const Rendering::Material* material, ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsv)
{
	ID3D11DeviceContext* context = dx->GetContext();

	// Rendering Option
	{
		context->RSSetState(dx->GetRasterizerStateCWDisableCulling());
	
		PixelShader::BindSamplerState(context, SamplerStateBindIndex::DefaultSamplerState, dx->GetSamplerStateLinear());
	
		context->OMSetDepthStencilState(dx->GetDepthStateGreaterEqualAndDisableDepthWrite(), 0);
		context->OMSetRenderTargets(1, &rtv, dsv);
	
		float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		context->OMSetBlendState(dx->GetBlendStateOpaque(), blendFactor, 0xffffffff);
	}

	_meshFilter->GetIndexBuffer()->IASetBuffer(context);
	_meshFilter->GetVertexBuffer()->IASetBuffer(context);

	// Render Sky
	{
		const auto& customShader = material->GetCustomShader().shaderGroup;

		ASSERT_MSG_IF(customShader.ableRender(), "Error,  this shader is invalid")
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

			context->DrawIndexed(_meshFilter->GetIndexCount(), 0, 0);

			context->VSSetShader(nullptr, nullptr, 0);
			context->PSSetShader(nullptr, nullptr, 0);

			if (gs)
				context->GSSetShader(nullptr, nullptr, 0);
		}
	}
}
