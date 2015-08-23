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

LightCulling::LightCulling() : 
	_computeShader(nullptr), _inputPointLightTransformBuffer(nullptr),
	_inputSpotLightTransformBuffer(nullptr), _inputSpotLightParamBuffer(nullptr)
{

}

LightCulling::~LightCulling()
{
	Destroy();
}

void LightCulling::AddInputBufferToList(GPGPU::DirectCompute::ComputeShader::InputShaderResourceBuffer*& outBuffer, uint idx, const ShaderResourceBuffer*& buffer)
{
	ComputeShader::InputShaderResourceBuffer inputBuffer;
	{
		inputBuffer.idx			= idx;
		inputBuffer.buffer		= buffer;
	}

	_inputBuffers.push_back(inputBuffer);
	outBuffer = &_inputBuffers.back();
}

void LightCulling::_Set_InputTexture_And_Append_To_InputTextureList(GPGPU::DirectCompute::ComputeShader::InputTexture** outTexture, uint idx, const Texture::Texture2D* texture)
{
	ComputeShader::InputTexture inputTex;
	{
		inputTex.idx		= idx;
		inputTex.texture	= texture;
	}

	_inputTextures.push_back(inputTex);

	if(outTexture)
		(*outTexture) = &_inputTextures.back();
}

void LightCulling::Initialize(const std::string& filePath, const std::string& mainFunc, bool useRenderBlendedMesh,
							  const Texture::DepthBuffer* opaqueDepthBuffer, const Texture::DepthBuffer* blendedDepthBuffer)
{
	Manager::LightManager* lightManager = Director::GetInstance()->GetCurrentScene()->GetLightManager();
	//Check duplicated input datas
	{
		for(const auto& iter : _inputBuffers)
		{
			if( iter.idx == (uint)InputBufferShaderIndex::PointLightRadiusWithCenter ||
				iter.idx == (uint)InputBufferShaderIndex::PointLightRadiusWithCenter ||
				iter.idx == (uint)InputBufferShaderIndex::PointLightRadiusWithCenter )
			{
				ASSERT_MSG("Error, duplicated input data");
			}
		}

		for(const auto& iter : _inputTextures)
		{
			if( iter.idx == (uint)InputTextureShaderIndex::InvetedOpaqueDepthBuffer ||
				iter.idx == (uint)InputTextureShaderIndex::InvetedBlendedDepthBuffer )
			{
				ASSERT_MSG("Error, duplicated input data");
			}
		}
	}

	ResourceManager* resourceManager = ResourceManager::GetInstance();
	auto shaderMgr = resourceManager->GetShaderManager();

	std::vector<Shader::ShaderMacro> macros;
	{
		ShaderMacro msaaMacro = Device::Director::GetInstance()->GetDirectX()->GetMSAAShaderMacro();
		macros.push_back(msaaMacro);

		macros.push_back(ShaderMacro("USE_COMPUTE_SHADER", ""));

		if(useRenderBlendedMesh)
			macros.push_back(ShaderMacro("ENABLE_BLEND", ""));
	}

	ID3DBlob* blob = shaderMgr->CreateBlob(filePath, "cs", "CS", false, &macros);

	ComputeShader::ThreadGroup threadGroup;
	UpdateThreadGroup(&threadGroup, false);
	_computeShader = new ComputeShader(threadGroup, blob);

	ASSERT_COND_MSG(_computeShader->Initialize(), "can not create compute shader");

	// Input Buffer Setting
	{
		// Point Light Transform
		uint idx = (uint)InputBufferShaderIndex::PointLightRadiusWithCenter;
		const ShaderResourceBuffer* srBuffer = lightManager->GetPointLightTransformBufferSR();
		AddInputBufferToList(_inputPointLightTransformBuffer, idx, srBuffer);
		
		// Spot Light Transform
		idx = (uint)InputBufferShaderIndex::SpotLightRadiusWithCenter;
		srBuffer = lightManager->GetSpotLightTransformBufferSR();
		AddInputBufferToList(_inputSpotLightTransformBuffer, idx, srBuffer);

		idx = (uint)InputBufferShaderIndex::SpotLightParam;
		srBuffer = lightManager->GetSpotLightParamBufferSR();
		AddInputBufferToList(_inputSpotLightParamBuffer, idx, srBuffer);

		// depth buffer
		{
			// Opaque Depth Buffer
			idx = (uint)InputTextureShaderIndex::InvetedOpaqueDepthBuffer;
			_Set_InputTexture_And_Append_To_InputTextureList(nullptr, idx, opaqueDepthBuffer);

			// Blended DepthBuffer (used in Transparency Rendering)
			if(useRenderBlendedMesh)
			{
				idx = (uint)InputTextureShaderIndex::InvetedBlendedDepthBuffer;
				_Set_InputTexture_And_Append_To_InputTextureList(nullptr, idx, blendedDepthBuffer);
			}
		}

		_computeShader->SetInputSRBuffers(_inputBuffers);
		_computeShader->SetInputTextures(_inputTextures);
	}

	_useBlendedMeshCulling = useRenderBlendedMesh;
}

unsigned int LightCulling::CalcMaxNumLightsInTile()
{
	const Math::Size<unsigned int>& size = Director::GetInstance()->GetBackBufferSize();
	const unsigned key = 16;

	return ( LightMaxNumInTile - ( key * ( size.h / 120 ) ) );
}

void LightCulling::Dispatch(const Device::DirectX* dx, const Buffer::ConstBuffer* tbrConstBuffer)
{
	ID3D11DeviceContext* context = dx->GetContext();

	if(tbrConstBuffer)
	{
		std::vector<ComputeShader::InputConstBuffer> tbrCB;
		{
			ComputeShader::InputConstBuffer icb;
			icb.buffer = tbrConstBuffer;
			icb.idx = (uint)ConstBufferShaderIndex::TBRParam;
		}

		_computeShader->SetInputConstBuffers(tbrCB);
	}
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
		return (unsigned int)((size+TileSize-1) / (float)TileSize);
	};

	const Math::Size<unsigned int>& size = Director::GetInstance()->GetBackBufferSize();

	unsigned int width	= CalcThreadLength(size.w);
	unsigned int height = CalcThreadLength(size.h);

	return Math::Size<unsigned int>(width, height);
}

void LightCulling::Destroy()
{
	_inputBuffers.clear();
	_inputTextures.clear();

	_inputPointLightTransformBuffer	= nullptr;
	_inputSpotLightTransformBuffer	= nullptr;
	_inputSpotLightParamBuffer		= nullptr;

	SAFE_DELETE(_computeShader);

	_useBlendedMeshCulling = false;
}