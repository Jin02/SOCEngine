#include "OnlyLightCulling.h"
#include "EngineShaderFactory.hpp"

using namespace Rendering::Light;
using namespace GPGPU::DirectCompute;
using namespace Rendering::Texture;
using namespace Rendering::Factory;
using namespace Rendering;

OnlyLightCulling::OnlyLightCulling() : LightCulling(), _lightIndexBuffer(nullptr)
{
}

OnlyLightCulling::~OnlyLightCulling()
{
}

void OnlyLightCulling::Initialize(
	const DepthBuffer* opaqueDepthBuffer,
	const DepthBuffer* blendedDepthBuffer,
	RenderType renderType,
	const std::vector<Shader::ShaderMacro>* opationalMacros)
{
	std::string path = "";
	{
		EngineFactory shaderFactory(nullptr); //only use FetchShaderFullPath
		shaderFactory.FetchShaderFullPath(path, "LightCulling_CS");

		ASSERT_COND_MSG(path.empty() == false, "Error, path is null");
	}

	LightCulling::Initialize(path, "OnlyLightCullingCS",
		opaqueDepthBuffer, blendedDepthBuffer,
		renderType, opationalMacros);

	// Ouput Buffer Setting
	{
		_lightIndexBuffer = new CSRWBuffer;
		Math::Size<unsigned int> size = CalcThreadGroupSize();

		uint num = CalcMaxNumLightsInTile() * size.w * size.h;
		_lightIndexBuffer->Initialize(DXGI_FORMAT_R16_UINT, 2, num);

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