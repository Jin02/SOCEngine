#include "FullScreen.h"
#include "EngineShaderFactory.hpp"

using namespace Device;
using namespace Rendering::PostProcessing;
using namespace Rendering::Shader;
using namespace Rendering::Texture;
using namespace Rendering::Manager;

void FullScreen::Initialize(Device::DirectX& dx, const InitParam& param, ShaderManager& shaderManager)
{
	std::string folderPath = "";
	{
		Factory::EngineShaderFactory pathFinder(nullptr);

		std::string path = pathFinder.FetchShaderFullPath(param.shaderFileName);

		Utility::String::ParseDirectory(path, &folderPath, nullptr, nullptr);
	}
	assert(folderPath.empty() == false);// "Error!, Invalid File Path"

#pragma region MacroKey
	std::vector<ShaderMacro> vsMacro;

	std::string vsKey	= "FullScreenVS";
	_psUniqueKey		= param.shaderFileName + ":" + param.psName;

	// VS
	{
		if (param.useViewInfo)
		{
			vsMacro.emplace_back("USE_VIEW_INFORMATION");
			vsKey += ":[ViewInfoMacro]:";
		}

		if (param.useMSAAMacro)
		{
			vsMacro.push_back(dx.GetMSAAShaderMacro());

			constexpr char* key = ":[MSAA]:";
			vsKey += key;
			_psUniqueKey += key;
		}
	}

	if (param.psMacros)
	{
		for (auto iter : (*param.psMacros))
			_psUniqueKey += ":[" + iter.GetName() + "/" + iter.GetDefinition() + "]";
	}
#pragma endregion MacroKey

#pragma region VertexShader
	if (shaderManager.GetPool<VertexShader>().Has(vsKey) == false)
	{
		// Setting Vertex Shader
		{
			std::vector<D3D11_INPUT_ELEMENT_DESC> nullDeclations;

			auto blob = shaderManager.GetCompiler().CreateBlob(folderPath, param.shaderFileName, "vs", "FullScreenVS", false, &vsMacro);
			_vs = std::make_shared<VertexShader>(blob, vsKey);
			_vs->Initialize(dx, nullDeclations);

			shaderManager.GetPool<VertexShader>().Add(vsKey, _vs);
		}
	}
#pragma endregion VertexShader

#pragma region PixelShader
	if (shaderManager.GetPool<PixelShader>().Has(_psUniqueKey) == false)
	{
		auto blob = shaderManager.GetCompiler().CreateBlob(folderPath, param.shaderFileName, "vs", "FullScreenVS", false, param.psMacros);
		_ps = std::make_shared<PixelShader>(blob, _psUniqueKey);
		_ps->Initialize(dx);

		shaderManager.GetPool<PixelShader>().Add(_psUniqueKey, _ps);
	}
#pragma endregion PixelShader
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

	_vs->BindShaderToContext(dx);
	_vs->BindInputLayoutToContext(dx);
	
	_ps->BindShaderToContext(dx);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	context->RSSetState( nullptr );

	context->Draw(3, 0);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	rtv = nullptr;
	context->OMSetRenderTargets(1, &rtv, nullDSV);
}