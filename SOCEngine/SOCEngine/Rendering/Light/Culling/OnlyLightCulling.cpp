#include "OnlyLightCulling.h"

using namespace Rendering::Light;
using namespace GPGPU::DirectCompute;
using namespace Rendering::Texture;

OnlyLightCulling::OnlyLightCulling() : LightCulling(), _lightIndexBuffer(nullptr)
{
}

OnlyLightCulling::~OnlyLightCulling()
{
}

void OnlyLightCulling::Initialize(const std::string& filePath, const std::string& mainFunc, bool useRenderBlendedMesh, const DepthBuffer* opaqueDepthBuffer, const DepthBuffer* blendedDepthBuffer)
{
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

	LightCulling::Initialize(filePath, mainFunc, useRenderBlendedMesh, opaqueDepthBuffer, blendedDepthBuffer);
	SetInputsToCS();
}

void OnlyLightCulling::Destroy()
{
	SAFE_DELETE(_lightIndexBuffer);

	LightCulling::Destroy();
}