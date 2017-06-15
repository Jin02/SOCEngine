#include "FullScreen.h"
#include "EngineShaderFactory.hpp"

using namespace Device;
using namespace Rendering::PostProcessing;
using namespace Rendering::Shader;
using namespace Rendering::Texture;
using namespace Rendering::Manager;

void FullScreen::Initialize(Device::DirectX& dx, const InitParam& param, ShaderManager& shaderManager)
{
	Factory::EngineShaderFactory pathFinder(nullptr);
	
	std::string path = pathFinder.FetchShaderFullPath(param.shaderFileName);
	
	std::string folderPath = "";
	bool success = Utility::String::ParseDirectory(path, &folderPath, nullptr, nullptr);
	assert(success);// "Error!, Invalid File Path"

	// Setting Vertex Shader
	{
		std::vector<D3D11_INPUT_ELEMENT_DESC> nullDeclations;

		// setting vs uniqueKey
		{
			_vsUniqueKey = "FullScreenVS";

			if (param.macros)
			{
				for (auto iter : (*param.macros))
					_vsUniqueKey += ":[" + iter.GetName() + "/" + iter.GetDefinition() + "]";
			}
		}

		auto blob = shaderManager.GetCompiler().CreateBlob(folderPath, param.shaderFileName, "vs", "FullScreenVS", false, param.macros);
		_vs = VertexShader(blob, _vsUniqueKey);
		_vs.Initialize(dx, nullDeclations);
	}

	// Setting Pixel Shader
	{
		auto blob = shaderManager.GetCompiler().CreateBlob(folderPath, param.shaderFileName, "vs", "FullScreenVS", false, param.macros);
		_ps = PixelShader(blob, param.shaderFileName + ":" + param.psName);
		_ps.Initialize(dx);
	}
}

void FullScreen::Render(Device::DirectX& dx, RenderTexture& outResultRT, bool useOutRTViewportSize)
{
	ID3D11DeviceContext* context = dx.GetContext();

	if(useOutRTViewportSize)
	{
		const auto& size = outResultRT.GetSize().Cast<float>();
		dx.SetViewport(Rect<float>(0.0f, 0.0f, size.w, size.h));
	}

	ID3D11RenderTargetView* rtv	= outResultRT.GetRaw();	

	ID3D11DepthStencilView* nullDSV = nullptr;
	context->OMSetRenderTargets(1, &rtv, nullDSV);
	context->OMSetDepthStencilState(dx.GetDepthStateDisableDepthTest(), 0x00);

	_vs.BindShaderToContext(dx);
	_vs.BindInputLayoutToContext(dx);
	
	_ps.BindShaderToContext(dx);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	context->RSSetState( nullptr );

	context->Draw(3, 0);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	rtv = nullptr;
	context->OMSetRenderTargets(1, &rtv, nullDSV);
}