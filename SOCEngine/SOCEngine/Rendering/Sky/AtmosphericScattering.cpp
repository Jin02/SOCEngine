#include "AtmosphericScattering.h"
#include "Director.h"
#include "ResourceManager.h"
#include "EngineShaderFactory.hpp"

using namespace Device;
using namespace Core;
using namespace Resource;
using namespace Rendering::Geometry;
using namespace Rendering::Manager;
using namespace Rendering::Buffer;
using namespace Rendering::Shader;
using namespace Rendering::Sky;
using namespace Rendering::Camera;
using namespace Math;

AtmosphericScattering::Material::Material(const std::string& name) : Rendering::Material(name, Type::Sky),
		_paramCB(nullptr), _ssTransformCB(nullptr)
{

}

AtmosphericScattering::Material::~Material()
{
	Destroy();

	SAFE_DELETE(_paramCB);
	SAFE_DELETE(_ssTransformCB);
}


void AtmosphericScattering::Material::Initialize(const Device::DirectX* dx, bool useGS)
{
	// Load Shader
	{
		auto shaderManager = ResourceManager::SharedInstance()->GetShaderManager();
		Factory::EngineFactory factory(shaderManager);
		factory.LoadShader("AtmosphericScattering", "VS", "PS", useGS ? "GS" : "", nullptr,
							&_customShaders.shaderGroup.vs, &_customShaders.shaderGroup.ps,
							&_customShaders.shaderGroup.gs);
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

		_ssTransformCB = new ConstBuffer;
		_ssTransformCB->Initialize(sizeof(SSTransform));
	}

	SetConstBufferUseBindIndex(1, _paramCB,			ShaderForm::Usage(true, useGS, false, true));
	SetConstBufferUseBindIndex(3, _ssTransformCB,	ShaderForm::Usage(true, useGS, false, false));
}

void AtmosphericScattering::Material::Destroy()
{
	_paramCB->Destroy();
	_ssTransformCB->Destroy();
}

void AtmosphericScattering::Material::UpdateTransform(const CameraForm* camera)
{
	SSTransform ssTransform;
	{
		Matrix& world = ssTransform.worldMat;
		
		Transform* transform = camera->GetOwner()->GetTransform();
		transform->FetchWorldMatrix(world);
	
		Matrix view;
		camera->GetViewMatrix(view, world);
	
		Vector3 camWorldPos(world._41, world._42, world._43);
	
		Matrix::Identity(world);
		world._41 = camWorldPos.x;
		world._42 = camWorldPos.y;
		world._43 = camWorldPos.z;
	
		world._11 = camera->GetFar();
		world._22 = camera->GetFar();
		world._33 = camera->GetFar();
		
		Matrix proj;
		camera->GetProjectionMatrix(proj, true);
	
		ssTransform.worldViewProjMat = world * view * proj;
	}

	if(_prevSSTransform == ssTransform)
		return;

	const Device::DirectX* dx = Device::Director::SharedInstance()->GetDirectX();
	
	_prevSSTransform = ssTransform;
	
	Matrix::Transpose(ssTransform.worldMat, ssTransform.worldMat);
	Matrix::Transpose(ssTransform.worldViewProjMat, ssTransform.worldViewProjMat);

	_ssTransformCB->UpdateSubResource(dx->GetContext(), &ssTransform);
}

void AtmosphericScattering::Material::UpdateParam(const Param& param)
{
	if(param == _prevParam)
		return;
	_prevParam = param;

	const Device::DirectX* dx = Device::Director::SharedInstance()->GetDirectX();
	_paramCB->UpdateSubResource(dx->GetContext(), &param);
}









AtmosphericScattering::AtmosphericScattering() : SkyForm(SkyForm::Type::Atmospheric), _material(nullptr)		
{
}

AtmosphericScattering::~AtmosphericScattering()
{
	Destroy();	
	SAFE_DELETE(_material);
}

void AtmosphericScattering::BindParamToShader(ID3D11DeviceContext* context,
									  const MeshCamera* mainCam,
									  const LightManager* lightMgr)
{
	VertexShader::BindConstBuffer(context,			ConstBufferBindIndex::TBRParam,							mainCam->GetTBRParamConstBuffer());
	VertexShader::BindConstBuffer(context,			ConstBufferBindIndex::Camera,							mainCam->GetCameraConstBuffer());
	PixelShader::BindConstBuffer(context,			ConstBufferBindIndex::TBRParam,							mainCam->GetTBRParamConstBuffer());
	PixelShader::BindConstBuffer(context,			ConstBufferBindIndex::Camera,							mainCam->GetCameraConstBuffer());
	
	VertexShader::BindShaderResourceBuffer(context,	TextureBindIndex::DirectionalLightDirXY,				lightMgr->GetDirectionalLightDirXYSRBuffer());
	VertexShader::BindShaderResourceBuffer(context,	TextureBindIndex::DirectionalLightOptionalParamIndex,	lightMgr->GetDirectionalLightOptionalParamIndexSRBuffer());
	PixelShader::BindShaderResourceBuffer(context,	TextureBindIndex::DirectionalLightDirXY,				lightMgr->GetDirectionalLightDirXYSRBuffer());
	PixelShader::BindShaderResourceBuffer(context,	TextureBindIndex::DirectionalLightOptionalParamIndex,	lightMgr->GetDirectionalLightOptionalParamIndexSRBuffer());

	if(_material->GetUseGS())
	{
		GeometryShader::BindConstBuffer(context,			ConstBufferBindIndex::TBRParam,							mainCam->GetTBRParamConstBuffer());
		GeometryShader::BindConstBuffer(context,			ConstBufferBindIndex::Camera,							mainCam->GetCameraConstBuffer());
		GeometryShader::BindShaderResourceBuffer(context,	TextureBindIndex::DirectionalLightDirXY,				lightMgr->GetDirectionalLightDirXYSRBuffer());
		GeometryShader::BindShaderResourceBuffer(context,	TextureBindIndex::DirectionalLightOptionalParamIndex,	lightMgr->GetDirectionalLightOptionalParamIndexSRBuffer());
	}
}

void AtmosphericScattering::UnBindParamToShader(ID3D11DeviceContext* context)
{
	VertexShader::BindConstBuffer(context,			ConstBufferBindIndex::TBRParam,							nullptr);
	VertexShader::BindConstBuffer(context,			ConstBufferBindIndex::Camera,							nullptr);
	PixelShader::BindConstBuffer(context,			ConstBufferBindIndex::Camera,							nullptr);
	PixelShader::BindConstBuffer(context,			ConstBufferBindIndex::TBRParam,							nullptr);
	
	VertexShader::BindShaderResourceBuffer(context,	TextureBindIndex::DirectionalLightDirXY,				nullptr);
	VertexShader::BindShaderResourceBuffer(context,	TextureBindIndex::DirectionalLightOptionalParamIndex,	nullptr);
	PixelShader::BindShaderResourceBuffer(context,	TextureBindIndex::DirectionalLightDirXY,				nullptr);
	PixelShader::BindShaderResourceBuffer(context,	TextureBindIndex::DirectionalLightOptionalParamIndex,	nullptr);

	if(_material->GetUseGS())
	{
		GeometryShader::BindConstBuffer(context,			ConstBufferBindIndex::TBRParam,							nullptr);
		GeometryShader::BindConstBuffer(context,			ConstBufferBindIndex::Camera,							nullptr);
		GeometryShader::BindShaderResourceBuffer(context,	TextureBindIndex::DirectionalLightDirXY,				nullptr);
		GeometryShader::BindShaderResourceBuffer(context,	TextureBindIndex::DirectionalLightOptionalParamIndex,	nullptr);
	}
}

void AtmosphericScattering::Initialize(const Device::DirectX* dx)
{
	SkyForm::Initialize();
	_material = new Material("@AtmosphericScattering");
	_material->Initialize(dx, false);
}

void AtmosphericScattering::_Render(const Device::DirectX* dx,
						   const Rendering::Texture::RenderTexture* out,
						   const MeshCamera* meshCam,
						   const LightManager* lightMgr)
{
	ID3D11DeviceContext* context = dx->GetContext();

	ID3D11RenderTargetView* rtv		= out->GetRenderTargetView();
	ID3D11DepthStencilView* dsv		= meshCam->GetOpaqueDepthBuffer()->GetDepthStencilView();

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

	BindParamToShader(context, meshCam, lightMgr);
	SkyForm::_Render(dx, _material, rtv, dsv);
	UnBindParamToShader(context);

	rtv = nullptr;
	dsv = nullptr;
	context->OMSetRenderTargets(1, &rtv, dsv);
}


void AtmosphericScattering::Render(const Device::DirectX* dx, const CameraForm* camera,
						   const Rendering::Texture::RenderTexture* out,
						   const Rendering::Texture::DepthBuffer* depthBuffer,
						   const LightManager* lightMgr)
{
	const MeshCamera* meshCam = dynamic_cast<const MeshCamera*>(camera);
	ASSERT_MSG_IF(meshCam, "Error, camera must be MeshCamera.");

	_Render(dx, out, meshCam, lightMgr);
}

void AtmosphericScattering::Render(const Device::DirectX* dx, const ReflectionProbe* probe,
						   const Rendering::Texture::TextureCube* out,
						   const Rendering::Texture::DepthBufferCube* depthBuffer,
						   const LightManager* lightMgr)
{
	// null
	ASSERT_MSG("not yet supported");
}

void AtmosphericScattering::Destroy()
{
	_material->Destroy();
}