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

void FullScreen::Initialize(const std::string& shaderFileName, const std::string& psName)
{
	Factory::EngineFactory shaderPathFinder(nullptr);
	
	std::string path = "";
	shaderPathFinder.FetchShaderFullPath(path, shaderFileName);

	
	std::string folderPath = "";
	bool success = Utility::String::ParseDirectory(path, &folderPath, nullptr, nullptr);
	ASSERT_COND_MSG(success, "Error!, Invalid File Path");


	std::vector<ShaderMacro> macros;
	{
		const ShaderMacro& msaaMacro = Director::GetInstance()->GetDirectX()->GetMSAAShaderMacro();
		macros.push_back(msaaMacro);
	}

	auto shaderManager = ResourceManager::GetInstance()->GetShaderManager();

	std::vector<D3D11_INPUT_ELEMENT_DESC> nullDeclations;

	{
		std::string command = Manager::ShaderManager::MakeCommand(shaderFileName, "FullScreenVS", "vs");
		_vertexShader = shaderManager->LoadVertexShader(folderPath, command, false, nullDeclations, &macros);
	}

	{
		std::string command = Manager::ShaderManager::MakeCommand(shaderFileName, psName, "ps");
		_pixelShader = shaderManager->LoadPixelShader(folderPath, command, false, &macros);
	}
}

void FullScreen::Render(const RenderTexture* rt)
{
	ID3D11RenderTargetView* rtv = rt->GetRenderTargetView();	

	auto dx = Director::GetInstance()->GetDirectX();
	ID3D11DeviceContext* context = dx->GetContext();

	ID3D11DepthStencilView* nullDSV = nullptr;
	context->OMSetRenderTargets(1, &rtv, nullDSV);

	context->OMSetDepthStencilState(dx->GetDepthStateDisableDepthTest(), 0x00);

	context->IASetInputLayout(nullptr);

	uint stride = 0;
	uint offset = 0;
	ID3D11Buffer* nullBuffer[] = {nullptr};

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	_vertexShader->SetShaderToContext(context);
	_vertexShader->SetInputLayoutToContext(context);
	
	_pixelShader->SetShaderToContext(context);

	_pixelShader->UpdateResources(context, nullptr, &_inputPSTextures, nullptr);

	ID3D11SamplerState* sampler = dx->GetSamplerStateLinear();
	context->PSSetSamplers(0, 1, &sampler);

	context->Draw(3, 0);
}

void FullScreen::Destroy()
{
	_vertexShader = nullptr;
	_pixelShader = nullptr;

	_inputPSTextures.clear();
}