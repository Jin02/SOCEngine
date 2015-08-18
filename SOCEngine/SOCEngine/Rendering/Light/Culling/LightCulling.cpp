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
	_computeShader(nullptr), _globalDataBuffer(nullptr), 
	_inputPointLightTransformBuffer(nullptr), _inputSpotLightTransformBuffer(nullptr), 
	_inputSpotLightParamBuffer(nullptr), _pointLightUpdateCounter(0),
	_spotLightUpdateCounter(0)
{

}

LightCulling::~LightCulling()
{
	Destroy();
}

void LightCulling::_Init_InputBuffer_And_Append_To_InputBufferList(GPGPU::DirectCompute::ComputeShader::InputBuffer*& outBuffer, uint idx, uint bufferStride, uint bufferElementNum, DXGI_FORMAT format)
{
	ComputeShader::InputBuffer inputBufferElement;
	const void* nullData[] = {nullptr, };

	ComputeShader::InputBuffer inputBuffer;
	{
		inputBuffer.idx			= idx;
		inputBuffer.buffer		= new CSInputBuffer;

		auto& buffer = inputBuffer.buffer;
		buffer->Initialize(bufferStride, bufferElementNum, format, nullData);
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

	_globalDataBuffer = new ConstBuffer;
	ASSERT_COND_MSG(_globalDataBuffer->Initialize(sizeof(GlobalData)), "can not create const buffer");

	// Input Buffer Setting
	{
		ComputeShader::InputBuffer inputBufferElement;
		const void* nullData[] = {nullptr, };

		// Point Light Transform
		uint idx = (uint)InputBufferShaderIndex::PointLightRadiusWithCenter;
		_Init_InputBuffer_And_Append_To_InputBufferList(_inputPointLightTransformBuffer, idx, sizeof(Math::Vector4), POINT_LIGHT_BUFFER_MAX_NUM, DXGI_FORMAT_R32G32B32A32_FLOAT);

		// Spot Light Transform
		idx = (uint)InputBufferShaderIndex::SpotLightRadiusWithCenter;
		_Init_InputBuffer_And_Append_To_InputBufferList(_inputSpotLightTransformBuffer, idx, sizeof(Math::Vector4), SPOT_LIGHT_BUFFER_MAX_NUM, DXGI_FORMAT_R32G32B32A32_FLOAT);

		idx = (uint)InputBufferShaderIndex::SpotLightParam;
		_Init_InputBuffer_And_Append_To_InputBufferList(_inputSpotLightParamBuffer, idx, sizeof(Math::Vector4) / 2, SPOT_LIGHT_BUFFER_MAX_NUM, DXGI_FORMAT_R16G16B16A16_FLOAT);

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

		_computeShader->SetInputBuffers(_inputBuffers);
		_computeShader->SetInputTextures(_inputTextures);
	}

	_useBlendedMeshCulling = useRenderBlendedMesh;
}

unsigned int LightCulling::CalcMaxNumLightsInTile()
{
	const Math::Size<unsigned int>& winSize = Director::GetInstance()->GetWindowSize();
	const unsigned key = 16;

	return ( LightMaxNumInTile - ( key * ( winSize.h / 120 ) ) );
}

void LightCulling::UpdateInputBuffers(const Device::DirectX* dx, const GlobalData* globalData, const Rendering::Manager::LightManager* lightManager)
{
	ID3D11DeviceContext* context = dx->GetContext();
	
	if(globalData)
		_globalDataBuffer->Update(context, globalData);

	// Point Light
	{
		uint updateCounter = lightManager->GetPointLightUpdateCounter();
		if(updateCounter != _pointLightUpdateCounter)
		{
			const void* buffer = lightManager->GetPointLightTransformBuffer();
			_inputPointLightTransformBuffer->buffer->Update(context, buffer);
			
			_pointLightUpdateCounter = updateCounter;
		}
	}

	// Spot Light
	{
		uint updateCounter = lightManager->GetSpotLightUpdateCounter();
		if(updateCounter != _spotLightUpdateCounter)
		{
			const void* buffer = lightManager->GetSpotLightTransformBuffer();
			_inputSpotLightTransformBuffer->buffer->Update(context, buffer);

			buffer = lightManager->GetSpotLightParamBuffer();
			_inputSpotLightParamBuffer->buffer->Update(context, buffer);

			_spotLightUpdateCounter = updateCounter;
		}
	}
}

void LightCulling::Dispatch(const Device::DirectX* dx)
{
	ID3D11DeviceContext* context = dx->GetContext();
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

	const Math::Size<unsigned int>& winSize = Director::GetInstance()->GetWindowSize();

	unsigned int width	= CalcThreadLength(winSize.w);
	unsigned int height = CalcThreadLength(winSize.h);

	return Math::Size<unsigned int>(width, height);
}

void LightCulling::Destroy()
{
	for(auto& iter : _inputBuffers)
		SAFE_DELETE(iter.buffer);

	_inputBuffers.clear();
	_inputTextures.clear();

	_inputPointLightTransformBuffer	= nullptr;
	_inputSpotLightTransformBuffer	= nullptr;
	_inputSpotLightParamBuffer		= nullptr;

	SAFE_DELETE(_computeShader);
	SAFE_DELETE(_globalDataBuffer);

	_useBlendedMeshCulling = false;
}