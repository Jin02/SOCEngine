#include "SkyScattering.h"
#include "Director.h"
#include "ResourceManager.h"
#include "EngineShaderFactory.hpp"

using namespace Device;
using namespace Resource;
using namespace Rendering;
using namespace Rendering::Manager;
using namespace Rendering::Buffer;
using namespace Rendering::Shader;
using namespace Rendering::Sky;

SkyScattering::SkyScattering()
	: _vs(nullptr), _ps(nullptr), _paramCB(nullptr)
{
}

SkyScattering::~SkyScattering()
{
	Destroy();	
}

void SkyScattering::Initialize()
{
	// Load Shader
	{
		Factory::EngineFactory shaderPathFinder(nullptr);
	
		const std::string shaderFileName = "SkyScattering";

		std::string path = "";
		shaderPathFinder.FetchShaderFullPath(path, shaderFileName);
	
		std::string folderPath = "";
		bool success = Utility::String::ParseDirectory(path, &folderPath, nullptr, nullptr);
		ASSERT_MSG_IF(success, "Error!, Invalid File Path");

		auto shaderManager = ResourceManager::SharedInstance()->GetShaderManager();

		// Setting Vertex Shader
		{
			std::vector<D3D11_INPUT_ELEMENT_DESC> nullDeclations;
			std::string command = Manager::ShaderManager::MakePartlyCommand(shaderFileName, "SkyScattering_InFullScreen_VS");

			_vs = shaderManager->LoadVertexShader(folderPath, command, false, nullDeclations, nullptr, nullptr);
		}

		// Setting Pixel Shader
		{
			std::string command = Manager::ShaderManager::MakePartlyCommand(shaderFileName, "SkyScattering_InFullScreen_PS");
			_ps = shaderManager->LoadPixelShader(folderPath, command, false, nullptr);
		}
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
	}
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

	ID3D11RenderTargetView* rtv		= out->GetRenderTargetView();
	ID3D11DepthStencilView* nullDSV	= nullptr;
	context->OMSetRenderTargets(1, &rtv, nullDSV);

	context->OMSetDepthStencilState(dx->GetDepthStateDisableDepthTest(), 0x00);

	_vs->BindShaderToContext(context);
	_vs->BindInputLayoutToContext(context);

	_ps->BindShaderToContext(context);

	VertexShader::BindConstBuffer(context,			ConstBufferBindIndex::TBRParam,							mainCam->GetTBRParamConstBuffer());
	PixelShader::BindConstBuffer(context,			ConstBufferBindIndex::TBRParam,							mainCam->GetTBRParamConstBuffer());
	VertexShader::BindConstBuffer(context,			ConstBufferBindIndex::Camera,							mainCam->GetCameraConstBuffer());
	PixelShader::BindConstBuffer(context,			ConstBufferBindIndex::Camera,							mainCam->GetCameraConstBuffer());
	VertexShader::BindConstBuffer(context,			ConstBufferBindIndex(1),								_paramCB);
	PixelShader::BindConstBuffer(context,			ConstBufferBindIndex(1),								_paramCB);
	
	VertexShader::BindShaderResourceBuffer(context,	TextureBindIndex::DirectionalLightDirXY,				lightMgr->GetDirectionalLightDirXYSRBuffer());
	VertexShader::BindShaderResourceBuffer(context,	TextureBindIndex::DirectionalLightOptionalParamIndex,	lightMgr->GetDirectionalLightOptionalParamIndexSRBuffer());
	PixelShader::BindShaderResourceBuffer(context,	TextureBindIndex::DirectionalLightDirXY,				lightMgr->GetDirectionalLightDirXYSRBuffer());
	PixelShader::BindShaderResourceBuffer(context,	TextureBindIndex::DirectionalLightOptionalParamIndex,	lightMgr->GetDirectionalLightOptionalParamIndexSRBuffer());

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	context->RSSetState( nullptr );

	context->Draw(3, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	VertexShader::BindConstBuffer(context,			ConstBufferBindIndex::TBRParam,							nullptr);
	PixelShader::BindConstBuffer(context,			ConstBufferBindIndex::TBRParam,							nullptr);
	VertexShader::BindConstBuffer(context,			ConstBufferBindIndex::Camera,							nullptr);
	PixelShader::BindConstBuffer(context,			ConstBufferBindIndex::Camera,							nullptr);
	VertexShader::BindConstBuffer(context,			ConstBufferBindIndex(1),								nullptr);
	PixelShader::BindConstBuffer(context,			ConstBufferBindIndex(1),								nullptr);

	VertexShader::BindShaderResourceBuffer(context,	TextureBindIndex::DirectionalLightDirXY,				nullptr);
	VertexShader::BindShaderResourceBuffer(context,	TextureBindIndex::DirectionalLightOptionalParamIndex,	nullptr);
	PixelShader::BindShaderResourceBuffer(context,	TextureBindIndex::DirectionalLightDirXY,				nullptr);
	PixelShader::BindShaderResourceBuffer(context,	TextureBindIndex::DirectionalLightOptionalParamIndex,	nullptr);


	rtv = nullptr;
	context->OMSetRenderTargets(1, &rtv, nullDSV);
}

void SkyScattering::Destroy()
{
	_paramCB->Destroy();
}