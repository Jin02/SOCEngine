#include "OnlyLightCulling.h"
#include "EngineShaderFactory.hpp"

using namespace Rendering::Light;
using namespace GPGPU::DirectCompute;
using namespace Rendering::Texture;
using namespace Rendering::Factory;
using namespace Rendering;
using namespace Rendering::Shader;

OnlyLightCulling::OnlyLightCulling() : LightCulling(), _lightIndexBuffer(nullptr)
{
}

OnlyLightCulling::~OnlyLightCulling()
{
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

		ASSERT_COND_MSG(path.empty() == false, "Error, path is null");
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
		_lightIndexBuffer = new CSRWBuffer;
		Math::Size<unsigned int> size = CalcThreadGroupSize();

		uint num = CalcMaxNumLightsInTile() * size.w * size.h;
		_lightIndexBuffer->Initialize(DXGI_FORMAT_R32_UINT, 4, num);

		ComputeShader::Output outputBuffer;
		{
			outputBuffer.idx	= (uint)OutputBufferShaderIndex::LightIndexBuffer;
			outputBuffer.output	= _lightIndexBuffer;
		}

		std::vector<ComputeShader::Output> outputs;
		outputs.push_back(outputBuffer);

		SetOuputBuferToCS(outputs);
	}

	SetInputsToCS();
}

void OnlyLightCulling::Destroy()
{
	SAFE_DELETE(_lightIndexBuffer);

	LightCulling::Destroy();
}