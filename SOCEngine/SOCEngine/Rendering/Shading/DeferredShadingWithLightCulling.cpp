#include "DeferredShadingWithLightCulling.h"
#include "EngineShaderFactory.hpp"

using namespace Device;
using namespace Core;
using namespace Rendering;
using namespace Rendering::Light;
using namespace Rendering::Buffer;
using namespace Rendering::Shader;
using namespace GPGPU::DirectCompute;

DeferredShadingWithLightCulling::DeferredShadingWithLightCulling() : 
	_offScreen(nullptr), _inputPointLightColorBuffer(nullptr),
	_inputSpotLightColorBuffer(nullptr), _inputDirectionalLightColorBuffer(nullptr),
	_inputDirectionalLightParamBuffer(nullptr), _inputDirectionalLightTransformBuffer(nullptr),
	_directionalLightUpdateCounter(0)
{
}

DeferredShadingWithLightCulling::~DeferredShadingWithLightCulling()
{
	Destory();
}

void DeferredShadingWithLightCulling::Initialize(const Texture::DepthBuffer* opaqueDepthBuffer, const Texture::RenderTexture* gbuffer_albedo_metallic,  const Texture::RenderTexture* gbuffer_specular_fresnel0,  const Texture::RenderTexture* gbuffer_normal_roughness,  const Math::Size<uint>& size)
{
	std::string filePath = "";
	{
		Factory::EngineFactory pathFind(nullptr);
		pathFind.FetchShaderFullPath(filePath, "TileBasedDeferredShading");

		ASSERT_COND_MSG(filePath.empty() == false, "Error, File path is empty");
	}

	// Input buffer
	{
		// Point Light Color
		{
			uint idx = (uint)InputBufferShaderIndex::PointLightColor;
			_Init_InputBuffer_And_Append_To_InputBufferList(_inputPointLightColorBuffer, idx, 4, POINT_LIGHT_BUFFER_MAX_NUM, DXGI_FORMAT_R8G8B8A8_UNORM);
		}

		// Spot Light Color
		{
			uint idx = (uint)InputBufferShaderIndex::SpotLightColor;
			_Init_InputBuffer_And_Append_To_InputBufferList(_inputSpotLightColorBuffer, idx, 4, SPOT_LIGHT_BUFFER_MAX_NUM, DXGI_FORMAT_R8G8B8A8_UNORM);
		}

		// Directional Light
		{
			// Center With DirZ
			{
				uint idx = (uint)InputBufferShaderIndex::DirectionalLightCenterWithDirZ;
				_Init_InputBuffer_And_Append_To_InputBufferList(_inputDirectionalLightTransformBuffer, idx, sizeof(Math::Vector4), DIRECTIONAL_LIGHT_BUFFER_MAX_NUM, DXGI_FORMAT_R32G32B32A32_FLOAT);
			}

			// Color
			{
				uint idx = (uint)InputBufferShaderIndex::DirectionalLightColor;
				_Init_InputBuffer_And_Append_To_InputBufferList(_inputDirectionalLightColorBuffer, idx, 4, DIRECTIONAL_LIGHT_BUFFER_MAX_NUM, DXGI_FORMAT_R8G8B8A8_UNORM);
			}

			// Param half / DirX, DirY
			{
				uint idx = (uint)InputBufferShaderIndex::DirectionalLightParam;
				_Init_InputBuffer_And_Append_To_InputBufferList(_inputDirectionalLightParamBuffer, idx, sizeof(Math::Vector2) / 2, DIRECTIONAL_LIGHT_BUFFER_MAX_NUM, DXGI_FORMAT_R16G16_FLOAT);
			}
		}
	}

	// Input Texture
	{
		// Albedo_Metallic
		{
			uint idx = (uint)InputTextureShaderIndex::GBuffer_Albedo_Metallic;
			_Set_InputTexture_And_Append_To_InputTextureList(nullptr, idx, gbuffer_albedo_metallic);
		}

		// Specular_Fresnel0
		{
			uint idx = (uint)InputTextureShaderIndex::GBuffer_Specular_Fresnel0;
			_Set_InputTexture_And_Append_To_InputTextureList(nullptr, idx, gbuffer_specular_fresnel0);
		}

		// Normal_Roughness
		{
			uint idx = (uint)InputTextureShaderIndex::GBuffer_Normal_Roughness;
			_Set_InputTexture_And_Append_To_InputTextureList(nullptr, idx, gbuffer_normal_roughness);
		}
	}

	// Offscreen
	{
		_offScreen = new CSRWTexture;
		_offScreen->Initialize(size, DXGI_FORMAT_R8G8B8A8_UNORM, 0);

		ComputeShader::Output output;
		{
			output.idx		= (uint)OutputBufferShaderIndex::OutScreen;
			output.output	= _offScreen;
		}

		std::vector<ComputeShader::Output> outputs;
		outputs.push_back(output);

		SetOuputBuferToComputeShader(outputs);
	}

	LightCulling::Initialize(filePath, "CS", false, opaqueDepthBuffer, nullptr);
}

void DeferredShadingWithLightCulling::UpdateInputDatas( const Device::DirectX* dx, 
														const GlobalData* globalData,
														const Rendering::Manager::LightManager* lightManager)
{
	ID3D11DeviceContext* context = dx->GetContext();

	// Directional Light
	{
		uint updateCounter = lightManager->GetDirectionalLightUpdateCounter();
		if(updateCounter != _directionalLightUpdateCounter)
		{
			//center, dirZ
			{
				const void* buffer = lightManager->GetDirectionalLightTransformBuffer();
				_inputDirectionalLightTransformBuffer->buffer->Update(context, buffer);
			}

			//color
			{
				const void* buffer = lightManager->GetDirectionalLightColorBuffer();
				_inputDirectionalLightColorBuffer->buffer->Update(context, buffer);
			}

			//param(dirX, dirY)
			{
				const void* buffer = lightManager->GetDirectionalLightParamBuffer();
				_inputDirectionalLightParamBuffer->buffer->Update(context, buffer);
			}

			_directionalLightUpdateCounter = updateCounter;
		}
	}

	//Light Culling 전용 UpdateCounter는 LightCulling에서 처리함
	//이 함수 최 하단의 UpdateInputDatas에서 호출되고 처리될테니 걱정 ㄴㄴ

	// Point Light Color Buffer
	{
		uint updateCounter = lightManager->GetPointLightUpdateCounter();
		if(updateCounter != GetPointLightUpdateCounter())
		{
			const void* buffer = lightManager->GetPointLightColorBuffer();
			_inputPointLightColorBuffer->buffer->Update(context, buffer);
		}
	}

	// Spot Light Color Buffer
	{
		uint updateCounter = lightManager->GetSpotLightUpdateCounter();
		if(updateCounter != GetSpotLightUpdateCounter())
		{
			const void* buffer = lightManager->GetSpotLightColorBuffer();
			_inputSpotLightColorBuffer->buffer->Update(context, buffer);
		}
	}

	LightCulling::UpdateInputBuffers(dx, globalData, lightManager);
}

void DeferredShadingWithLightCulling::Destory()
{
	//Input Buffer들은 LightCulling에서 지워짐
	//idx 체크용 변수들만 null 처리하면 됨

	_inputPointLightColorBuffer				= nullptr;
	_inputSpotLightColorBuffer				= nullptr;
	_inputDirectionalLightTransformBuffer	= nullptr;
	_inputDirectionalLightColorBuffer		= nullptr;
	_inputDirectionalLightParamBuffer		= nullptr;

	_directionalLightUpdateCounter = 0;

	SAFE_DELETE(_offScreen);
	LightCulling::Destroy();
}