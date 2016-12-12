#include "FullScreen.h"
#include "EngineShaderFactory.hpp"
#include "Director.h"
#include "ResourceManager.h"
#include "Utility.h"

using namespace Device;
using namespace Rendering::PostProcessing;
using namespace Rendering::Shader;
using namespace Rendering::Texture;
using namespace Resource;

FullScreen::FullScreen()
	: _vertexShader(nullptr), _pixelShader(nullptr)
{
}

FullScreen::~FullScreen()
{
	Destroy();
}

void FullScreen::Initialize(const std::string& shaderFileName, const std::string& psName, const std::vector<ShaderMacro>* macros)
{
	Destroy();
	Factory::EngineFactory shaderPathFinder(nullptr);
	
	std::string path = "";
	shaderPathFinder.FetchShaderFullPath(path, shaderFileName);
	
	std::string folderPath = "";
	bool success = Utility::String::ParseDirectory(path, &folderPath, nullptr, nullptr);
	ASSERT_MSG_IF(success, "Error!, Invalid File Path");

	auto shaderManager = ResourceManager::SharedInstance()->GetShaderManager();

	// Setting Vertex Shader
	{
		std::vector<D3D11_INPUT_ELEMENT_DESC> nullDeclations;
		std::string command = Manager::ShaderManager::MakePartlyCommand(shaderFileName, "FullScreenVS");
		_vertexShader = shaderManager->LoadVertexShader(folderPath, command, false, nullDeclations, macros);
	}

	// Setting Pixel Shader
	{
		std::string command = Manager::ShaderManager::MakePartlyCommand(shaderFileName, psName);
		_pixelShader = shaderManager->LoadPixelShader(folderPath, command, false, macros);
	}
}

void FullScreen::Render(const DirectX* dx, const RenderTexture* outResultRT,
						bool useOutRTViewportSize)
{
	ID3D11DeviceContext* context	= dx->GetContext();

	if(useOutRTViewportSize)
	{
		D3D11_VIEWPORT viewport;
		viewport.TopLeftX	= 0.0f;
		viewport.TopLeftY	= 0.0f;
		viewport.MinDepth	= 0.0f;
		viewport.MaxDepth	= 1.0f;
		viewport.Width		= outResultRT->GetSize().w;
		viewport.Height		= outResultRT->GetSize().h;
		
		context->RSSetViewports(1, &viewport);
	}

	ID3D11RenderTargetView* rtv		= outResultRT->GetRenderTargetView();	

	ID3D11DepthStencilView* nullDSV = nullptr;
	context->OMSetRenderTargets(1, &rtv, nullDSV);
	context->OMSetDepthStencilState(dx->GetDepthStateDisableDepthTest(), 0x00);

	_vertexShader->BindShaderToContext(context);
	_vertexShader->BindInputLayoutToContext(context);
	_vertexShader->BindResourcesToContext(context, &_inputConstBuffers, &_inputTextures, &_inputSRBuffers);
	
	_pixelShader->BindShaderToContext(context);
	_pixelShader->BindResourcesToContext(context, &_inputConstBuffers, &_inputTextures, &_inputSRBuffers);

	PixelShader::BindSamplerState(context, SamplerStateBindIndex::DefaultSamplerState, dx->GetSamplerStateLinear());
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	context->RSSetState( nullptr );

	context->Draw(3, 0);

	//context->RSSetState( dx->GetRasterizerStateCCWDefaultState() );
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	rtv = nullptr;
	context->OMSetRenderTargets(1, &rtv, nullDSV);
}

void FullScreen::Destroy()
{
	auto shaderManager = ResourceManager::SharedInstance()->GetShaderManager();
	if(_vertexShader)
	{
		shaderManager->DeleteShader(_vertexShader->GetKey());
		_vertexShader	= nullptr;
	}
	if(_pixelShader)
	{
		shaderManager->DeleteShader(_pixelShader->GetKey());
		_pixelShader	= nullptr;
	}

	_inputTextures.clear();
	_inputConstBuffers.clear();
	_inputSRBuffers.clear();
}
