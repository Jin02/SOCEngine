#include "SkyScattering.h"
#include "Director.h"
#include "ResourceManager.h"
#include "EngineShaderFactory.hpp"

using namespace Device;
using namespace Core;
using namespace Resource;
using namespace Rendering;
using namespace Rendering::Geometry;
using namespace Rendering::Manager;
using namespace Rendering::Buffer;
using namespace Rendering::Shader;
using namespace Rendering::Sky;
using namespace Math;

SkyScattering::SkyScattering()
	: _vs(nullptr), _ps(nullptr), _paramCB(nullptr), _meshFilter(nullptr),
	_worldMatCB(nullptr)
{
}

SkyScattering::~SkyScattering()
{
	Destroy();	

	SAFE_DELETE(_meshFilter);
	SAFE_DELETE(_worldMatCB);
	SAFE_DELETE(_paramCB);
}

void SkyScattering::Initialize()
{
	// Load Shader
	{
		auto shaderManager = ResourceManager::SharedInstance()->GetShaderManager();
		Factory::EngineFactory factory(shaderManager);
		factory.LoadShader("SkyScattering", "SkyScattering_InFullScreen_VS", "SkyScattering_InFullScreen_PS", "", nullptr, &_vs, &_ps, nullptr);
	}

	// Const Buffer
	{
		_paramCB = new ConstBuffer;
		_paramCB->Initialize(sizeof(Param));

		Param param;
		param.dlIndex = 0;
		param.luminance = Math::Common::FloatToHalf(1.0f);
		param.mieCoefficient = 0.005f;
		param.mieDirectionalG = 0.8f;
		param.rayleigh = Math::Common::FloatToHalf(2.0f);
		param.turbidity = Math::Common::FloatToHalf(10.0f);
		UpdateParam(param);

		_worldMatCB = new ConstBuffer;
		_worldMatCB->Initialize(sizeof(Matrix));
	}

	// Create Mesh
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
}

void SkyScattering::UpdateWorldMat(const Camera::CameraForm* mainCam)
{
	Matrix world;
	
	Transform* transform = mainCam->GetOwner()->GetTransform();
	transform->FetchWorldMatrix(world);
	
	if(_prevWorldMat == world)
		return;

	const Device::DirectX* dx = Device::Director::SharedInstance()->GetDirectX();
	
	_prevWorldMat = world;
	
	Matrix::Transpose(world, world);
	_worldMatCB->UpdateSubResource(dx->GetContext(), &world);
}

void SkyScattering::UpdateParam(const Param& param)
{
	if(param == _prevParam)
		return;

	const Device::DirectX* dx = Device::Director::SharedInstance()->GetDirectX();

	_prevParam = param;
	_paramCB->UpdateSubResource(dx->GetContext(), &param);
}

void SkyScattering::Render(const Device::DirectX* dx,
						   const Texture::RenderTexture* out,
						   const Camera::MeshCamera* mainCam,
						   const Manager::LightManager* lightMgr)
{
	ID3D11DeviceContext* context = dx->GetContext();

	ID3D11RenderTargetView* rtv		= out->GetRenderTargetView();
	ID3D11DepthStencilView* dsv		= mainCam->GetOpaqueDepthBuffer()->GetDepthStencilView();

	// Rendering Option
	{
		context->RSSetState(dx->GetRasterizerStateCWDisableCulling());
	
		PixelShader::BindSamplerState(context, SamplerStateBindIndex::DefaultSamplerState, dx->GetSamplerStateLinear());
	
		context->OMSetDepthStencilState(dx->GetDepthStateGreaterEqualAndDisableDepthWrite(), 0);
		context->OMSetRenderTargets(1, &rtv, dsv);
	
		float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		context->OMSetBlendState(dx->GetBlendStateOpaque(), blendFactor, 0xffffffff);
	}


	D3D11_VIEWPORT viewport;
	{
		viewport.TopLeftX	= 0.0f;
		viewport.TopLeftY	= 0.0f;
		viewport.MinDepth	= 0.0f;
		viewport.MaxDepth	= 1.0f;
		viewport.Width		= out->GetSize().w;
		viewport.Height		= out->GetSize().h;
	}
	context->RSSetViewports(1, &viewport);

	_vs->BindShaderToContext(context);
	_vs->BindInputLayoutToContext(context);

	_ps->BindShaderToContext(context);

	VertexShader::BindConstBuffer(context,			ConstBufferBindIndex::TBRParam,							mainCam->GetTBRParamConstBuffer());
	PixelShader::BindConstBuffer(context,			ConstBufferBindIndex::TBRParam,							mainCam->GetTBRParamConstBuffer());
	VertexShader::BindConstBuffer(context,			ConstBufferBindIndex::Camera,							mainCam->GetCameraConstBuffer());
	PixelShader::BindConstBuffer(context,			ConstBufferBindIndex::Camera,							mainCam->GetCameraConstBuffer());
	VertexShader::BindConstBuffer(context,			ConstBufferBindIndex(1),								_paramCB);
	PixelShader::BindConstBuffer(context,			ConstBufferBindIndex(1),								_paramCB);
	VertexShader::BindConstBuffer(context,			ConstBufferBindIndex(3),								_worldMatCB);
	PixelShader::BindConstBuffer(context,			ConstBufferBindIndex(3),								_worldMatCB);
	
	VertexShader::BindShaderResourceBuffer(context,	TextureBindIndex::DirectionalLightDirXY,				lightMgr->GetDirectionalLightDirXYSRBuffer());
	VertexShader::BindShaderResourceBuffer(context,	TextureBindIndex::DirectionalLightOptionalParamIndex,	lightMgr->GetDirectionalLightOptionalParamIndexSRBuffer());
	PixelShader::BindShaderResourceBuffer(context,	TextureBindIndex::DirectionalLightDirXY,				lightMgr->GetDirectionalLightDirXYSRBuffer());
	PixelShader::BindShaderResourceBuffer(context,	TextureBindIndex::DirectionalLightOptionalParamIndex,	lightMgr->GetDirectionalLightOptionalParamIndexSRBuffer());

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	_meshFilter->GetIndexBuffer()->IASetBuffer(context);
	_meshFilter->GetVertexBuffer()->IASetBuffer(context);
	context->DrawIndexed(_meshFilter->GetIndexCount(), 0, 0);

	_vs->UnBindBasicInputs(context);
	_ps->UnBindShaderToContext(context);

	VertexShader::BindConstBuffer(context,			ConstBufferBindIndex::TBRParam,							nullptr);
	PixelShader::BindConstBuffer(context,			ConstBufferBindIndex::TBRParam,							nullptr);
	VertexShader::BindConstBuffer(context,			ConstBufferBindIndex::Camera,							nullptr);
	PixelShader::BindConstBuffer(context,			ConstBufferBindIndex::Camera,							nullptr);
	VertexShader::BindConstBuffer(context,			ConstBufferBindIndex(1),								nullptr);
	PixelShader::BindConstBuffer(context,			ConstBufferBindIndex(1),								nullptr);
	VertexShader::BindConstBuffer(context,			ConstBufferBindIndex(3),								nullptr);
	PixelShader::BindConstBuffer(context,			ConstBufferBindIndex(3),								nullptr);

	VertexShader::BindShaderResourceBuffer(context,	TextureBindIndex::DirectionalLightDirXY,				nullptr);
	VertexShader::BindShaderResourceBuffer(context,	TextureBindIndex::DirectionalLightOptionalParamIndex,	nullptr);
	PixelShader::BindShaderResourceBuffer(context,	TextureBindIndex::DirectionalLightDirXY,				nullptr);
	PixelShader::BindShaderResourceBuffer(context,	TextureBindIndex::DirectionalLightOptionalParamIndex,	nullptr);

	rtv = nullptr;
	context->OMSetRenderTargets(1, &rtv, dsv);
}

void SkyScattering::Destroy()
{
	_paramCB->Destroy();
	_worldMatCB->Destroy();
}