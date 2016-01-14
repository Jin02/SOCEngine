#include "LightCulling.h"
#include "Director.h"

#include "ResourceManager.h"

#include "Utility.h"
#include "EngineShaderFactory.hpp"

using namespace Device;
using namespace Core;
using namespace Rendering;
using namespace Rendering::Light;
using namespace Rendering::Buffer;
using namespace Rendering::Shader;
using namespace GPGPU::DirectCompute;
using namespace Resource;
using namespace Rendering::TBDR;

LightCulling::LightCulling() : _computeShader(nullptr)
{

}

LightCulling::~LightCulling()
{
	Destroy();
}

void LightCulling::AddInputBufferToList(uint idx, const ShaderResourceBuffer* buffer)
{
	ShaderForm::InputShaderResourceBuffer inputBuffer;
	{
		inputBuffer.bindIndex	= idx;
		inputBuffer.srBuffer	= buffer;
	}

	_inputBuffers.push_back(inputBuffer);
}

void LightCulling::AddTextureToInputTextureList(uint idx, const Texture::Texture2D* texture)
{
	ShaderForm::InputTexture inputTex;
	{
		inputTex.bindIndex	= idx;
		inputTex.texture	= texture;
	}

	_inputTextures.push_back(inputTex);
}

void LightCulling::Initialize(const std::string& filePath, const std::string& mainFunc,
							  const Texture::DepthBuffer* opaqueDepthBuffer, const Texture::DepthBuffer* blendedDepthBuffer,
							  const std::vector<Shader::ShaderMacro>* opationalMacros)
{
	ResourceManager* resourceManager = ResourceManager::SharedInstance();
	auto shaderMgr = resourceManager->GetShaderManager();

	std::vector<Shader::ShaderMacro> macros;
	{
		ShaderMacro msaaMacro = Device::Director::SharedInstance()->GetDirectX()->GetMSAAShaderMacro();
		macros.push_back(msaaMacro);

		macros.push_back(ShaderMacro("USE_COMPUTE_SHADER", ""));

		if(blendedDepthBuffer)
			macros.push_back(ShaderMacro("ENABLE_BLEND", ""));

		if(opationalMacros)
			macros.insert(macros.end(), opationalMacros->begin(), opationalMacros->end());
	}

	ID3DBlob* blob = shaderMgr->CreateBlob(filePath, "cs", mainFunc, false, &macros);

	ComputeShader::ThreadGroup threadGroup;
	UpdateThreadGroup(&threadGroup, false);
	_computeShader = new ComputeShader(threadGroup, blob);

	ASSERT_COND_MSG(_computeShader->Initialize(), "can not create compute shader");
	Manager::LightManager* lightManager = Director::SharedInstance()->GetCurrentScene()->GetLightManager();

	// Input Buffer Setting
	{
		AddInputBufferToList(uint(TextureBindIndex::PointLightRadiusWithCenter),	lightManager->GetPointLightTransformSRBuffer());
		AddInputBufferToList(uint(TextureBindIndex::SpotLightRadiusWithCenter),		lightManager->GetSpotLightTransformSRBuffer());
		AddInputBufferToList(uint(TextureBindIndex::SpotLightParam),				lightManager->GetSpotLightParamSRBuffer());

		// depth buffer
		{
			// Opaque Depth Buffer
			AddTextureToInputTextureList(uint(TextureBindIndex::GBuffer_Depth), opaqueDepthBuffer);

			// Blended DepthBuffer (used in Transparency Rendering)
			if(blendedDepthBuffer)
				AddTextureToInputTextureList(uint(TextureBindIndex::GBuffer_BlendedDepth), blendedDepthBuffer);
		}
	}

	_useBlendedMeshCulling = blendedDepthBuffer != nullptr;
}

void LightCulling::SetInputsToCS()
{
	_computeShader->SetInputSRBuffers(_inputBuffers);
	_computeShader->SetInputTextures(_inputTextures);
}

// 화면 크기에 따라 유동적으로 타일 안 최대 빛 갯수를 계산한다.
unsigned int LightCulling::CalcMaxNumLightsInTile()
{
	const Math::Size<unsigned int>& size = Director::SharedInstance()->GetBackBufferSize();
	const uint key = LIGHT_CULLING_TILE_RES;

	return ( LIGHT_CULLING_LIGHT_MAX_COUNT_IN_TILE - ( key * ( size.h / 120 ) ) );
}

void LightCulling::Dispatch(const Device::DirectX* dx,
							const Buffer::ConstBuffer* tbrConstBuffer,
							const std::vector<ShaderForm::InputConstBuffer>* additionalConstBuffers)
{
	ID3D11DeviceContext* context = dx->GetContext();

	std::vector<ShaderForm::InputConstBuffer> inputConstBuffers;

	if(tbrConstBuffer)
	{
		ShaderForm::InputConstBuffer icb;

		icb.buffer		= tbrConstBuffer;
		icb.bindIndex	= (uint)ConstBufferBindIndex::TBRParam;
		inputConstBuffers.push_back(icb);
	}
	if(additionalConstBuffers)
		inputConstBuffers.insert(inputConstBuffers.end(), additionalConstBuffers->begin(), additionalConstBuffers->end());

	_computeShader->SetInputConstBuffers(inputConstBuffers);
	_computeShader->Dispatch(context);
}

void LightCulling::UpdateThreadGroup(ComputeShader::ThreadGroup* outThreadGroup, bool updateComputeShader)
{
	Math::Size<unsigned int> groupSize = CalcThreadGroupSize();
	ComputeShader::ThreadGroup threadGroup = ComputeShader::ThreadGroup(groupSize.w, groupSize.h, 1);

	if(outThreadGroup)
		(*outThreadGroup) = threadGroup;

	if(updateComputeShader)
		_computeShader->SetThreadGroupInfo(threadGroup);
}

const Math::Size<unsigned int> LightCulling::CalcThreadGroupSize() const
{
	auto CalcThreadLength = [](unsigned int size)
	{
		return (unsigned int)((size + LIGHT_CULLING_TILE_RES - 1) / (float)LIGHT_CULLING_TILE_RES);
	};

	const Math::Size<unsigned int>& size = Director::SharedInstance()->GetBackBufferSize();

	unsigned int width	= CalcThreadLength(size.w);
	unsigned int height = CalcThreadLength(size.h);

	return Math::Size<unsigned int>(width, height);
}

void LightCulling::Destroy()
{
	_inputBuffers.clear();
	_inputTextures.clear();

	SAFE_DELETE(_computeShader);

	_useBlendedMeshCulling = false;
}