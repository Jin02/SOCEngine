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
}

void FullScreen::Initialize(const std::string& shaderFileName, const std::string& psName, const std::vector<ShaderMacro>* macros)
{
	Factory::EngineFactory shaderPathFinder(nullptr);
	
	std::string path = "";
	shaderPathFinder.FetchShaderFullPath(path, shaderFileName);
	
	std::string folderPath = "";
	bool success = Utility::String::ParseDirectory(path, &folderPath, nullptr, nullptr);
	ASSERT_COND_MSG(success, "Error!, Invalid File Path");

	auto shaderManager = ResourceManager::GetInstance()->GetShaderManager();

	// Setting Vertex Shader
	{
		std::vector<D3D11_INPUT_ELEMENT_DESC> nullDeclations;
		std::string command = Manager::ShaderManager::MakePartlyCommand(shaderFileName, "FullScreenVS");
		_vertexShader = shaderManager->LoadVertexShader(folderPath, command, false, nullDeclations, nullptr);
	}

	// Setting Pixel Shader
	{
		std::string command = Manager::ShaderManager::MakePartlyCommand(shaderFileName, psName);
		_pixelShader = shaderManager->LoadPixelShader(folderPath, command, false, macros);
	}
}

void FullScreen::Render(const RenderTexture* outResultRT, ID3D11SamplerState* sampler)
{
	ID3D11RenderTargetView* rtv = outResultRT->GetRenderTargetView();	

	auto dx = Director::GetInstance()->GetDirectX();
	ID3D11DeviceContext* context = dx->GetContext();

	ID3D11DepthStencilView* nullDSV = nullptr;
	context->OMSetRenderTargets(1, &rtv, nullDSV);
	context->OMSetDepthStencilState(dx->GetDepthStateDisableDepthTest(), 0x00);

	uint stride = 0;
	uint offset = 0;
	ID3D11Buffer* nullBuffer[] = {nullptr};


	_vertexShader->BindShaderToContext(context);
	_vertexShader->BindInputLayoutToContext(context);
	
	_pixelShader->BindShaderToContext(context);
	_pixelShader->BindResourcesToContext(context, nullptr, &_inputPSTextures, nullptr);
	context->PSSetSamplers(
		(uint)Rendering::TBDR::InputSamplerStateBindSlotIndex::DefaultSamplerState,
		1, &sampler);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	context->RSSetState( nullptr );

	context->Draw(3, 0);

	//context->RSSetState( dx->GetRasterizerStateCCWDefaultState() );
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void FullScreen::Destroy()
{
	_vertexShader = nullptr;
	_pixelShader = nullptr;

	_inputPSTextures.clear();
}