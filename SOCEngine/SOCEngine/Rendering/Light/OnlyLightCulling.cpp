#include "OnlyLightCulling.h"
#include "EngineShaderFactory.hpp"

using namespace Rendering::Light;
using namespace GPGPU::DirectCompute;
using namespace Rendering::Texture;
using namespace Rendering::Factory;
using namespace Rendering;
using namespace Rendering::Shader;
using namespace Rendering::Buffer;
using namespace Rendering::View;

OnlyLightCulling::OnlyLightCulling()
	: LightCulling(), _shaderResourceBuffer(nullptr), _uav(nullptr)
{
}

OnlyLightCulling::~OnlyLightCulling()
{
	SAFE_DELETE(_shaderResourceBuffer);
	SAFE_DELETE(_uav);
}

void OnlyLightCulling::Initialize(
	const DepthBuffer* opaqueDepthBuffer,
	const DepthBuffer* blendedDepthBuffer,
	const std::vector<ShaderMacro>* opationalMacros)
{
	std::string path = "";
	{
		EngineFactory shaderFactory(nullptr); //only use FetchShaderFullPath
		shaderFactory.FetchShaderFullPath(path, "OnlyLightCullingCS");

		ASSERT_MSG_IF(path.empty() == false, "Error, path is null");
	}

	std::vector<ShaderMacro> macros;
	{
		// LightCulling시, Depth Bound 계산을 atomic 비교 방식을 사용함
		macros.push_back(ShaderMacro("USE_ATOMIC", ""));

		// Parallel 방식
		//macros.push_back(ShaderMacro("USE_PARALLEL", ""));

		if(opationalMacros)
			macros.insert(macros.end(), opationalMacros->begin(), opationalMacros->end());
	}

	LightCulling::Initialize(path, "OnlyLightCullingCS",
		opaqueDepthBuffer, blendedDepthBuffer, &macros);

	// Ouput Buffer Setting
	{
		Math::Size<unsigned int> size = CalcThreadGroupSize();
		uint num = CalcMaxNumLightsInTile() * size.w * size.h;

		_shaderResourceBuffer = new ShaderResourceBuffer;
		_shaderResourceBuffer->Initialize(4, num, DXGI_FORMAT_R32_UINT, nullptr, false, D3D11_BIND_UNORDERED_ACCESS, D3D11_USAGE_DEFAULT);

		_uav = new UnorderedAccessView;
		_uav->Initialize(DXGI_FORMAT_R32_UINT, num, _shaderResourceBuffer->GetBuffer(), D3D11_UAV_DIMENSION_BUFFER);

		ShaderForm::InputUnorderedAccessView outputBuffer;
		{
			outputBuffer.bindIndex	= (uint)UAVBindIndex::Lightculling_LightIndexBuffer;
			outputBuffer.uav		= _uav;
		}

		std::vector<ShaderForm::InputUnorderedAccessView> outputs;
		outputs.push_back(outputBuffer);

		SetOuputBuferToCS(outputs);
	}

	SetInputsToCS();
}

void OnlyLightCulling::Destroy()
{
	LightCulling::Destroy();

	_shaderResourceBuffer->Destroy();
	_uav->Destroy();
}