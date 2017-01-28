#pragma once

#include "Lights.h"
#include "Frustum.h"
#include "VectorHashMap.h"
#include "ShaderResourceBuffer.h"
#include "GPUUploadBuffer.hpp"

#include <functional>

namespace Rendering
{
	namespace Manager
	{
		class LightManager
		{
		public:
			struct Lights
			{
				uint				prevTransformUpdateCounter;
				Light::LightForm*	light;

				Lights(Light::LightForm* _light, uint prevTFCounter) : light(_light), prevTransformUpdateCounter(prevTFCounter) {}
				~Lights(){}
			};

		private:
			Structure::VectorHashMap<address, Lights>										_lights;
			Structure::VectorHashMap<address, Light::DirectionalLight*>						_directionalLights; // using for compute frustum
			Structure::VectorHashMap<address, Light::PointLight*>							_pointLights;
			Structure::VectorHashMap<address, Light::SpotLight*>							_spotLights;

			template<typename TransformType>
			struct LightBuffers
			{
				Buffer::GPUUploadBuffer<address, TransformType>	transformBuffer;
				Buffer::GPUUploadBuffer<address, uint>			colorBuffer;
				Buffer::GPUUploadBuffer<address, uint>			optionalParamIndexBuffer;
			};

			LightBuffers<Light::LightForm::LightTransformBuffer>*							_pointLight;
			LightBuffers<Light::DirectionalLight::Param>*									_directionalLight;
			LightBuffers<Light::LightForm::LightTransformBuffer>*							_spotLight;
			Buffer::GPUUploadBuffer<address, Light::SpotLight::Param>*						_spotLightParamBuffer;

			bool																			_forceUpdateDL;
			bool																			_forceUpdatePL;
			bool																			_forceUpdateSL;

		public:
			LightManager(void);
			~LightManager(void);

		private:
			void UpdateSRBufferUsingMapDiscard(	ID3D11DeviceContext* context,
												const std::function<ushort(const Light::LightForm*)>& getShadowIndexInEachShadowLights,
												const std::function<uchar(const Light::LightForm*)>& getLightShaftIndex);

		public:
			void InitializeAllShaderResourceBuffer();
			void DestroyAllShaderReourceBuffer();

			uint Add(Light::LightForm*& light);
			void UpdateSRBuffer(const Device::DirectX* dx,
								const std::function<ushort(const Light::LightForm*)>& getShadowIndexInEachShadowLights,
								const std::function<uchar(const Light::LightForm*)>& getLightShaftIndex);
			void ComputeDirectionalLightViewProj(const Intersection::BoundBox& sceneBoundBox, float directionalLightShadowMapResolution);

			bool Has(Light::LightForm*& light) const;
			uint FetchLightIndexInEachLights(const Light::LightForm* light) const;

			void Delete(const Light::LightForm*& light);
			void DeleteAll();
			void Destroy();

			uint GetPackedLightCount() const;
			void BindResources(const Device::DirectX* dx, bool bindVS, bool bindGS, bool bindPS) const;
			void UnbindResources(const Device::DirectX* dx, bool bindVS, bool bindGS, bool bindPS) const;

		public:
			GET_ACCESSOR(PointLightTransformSRBuffer,						const Buffer::ShaderResourceBuffer*,	_pointLight->transformBuffer.GetShaderResourceBuffer());
			GET_ACCESSOR(PointLightColorSRBuffer,							const Buffer::ShaderResourceBuffer*,	_pointLight->colorBuffer.GetShaderResourceBuffer());
			GET_ACCESSOR(PointLightOptionalParamIndexSRBuffer,				const Buffer::ShaderResourceBuffer*,	_pointLight->optionalParamIndexBuffer.GetShaderResourceBuffer());

			GET_ACCESSOR(DirectionalLightDirXYSRBuffer,						const Buffer::ShaderResourceBuffer*,	_directionalLight->transformBuffer.GetShaderResourceBuffer());
			GET_ACCESSOR(DirectionalLightColorSRBuffer,						const Buffer::ShaderResourceBuffer*,	_directionalLight->colorBuffer.GetShaderResourceBuffer());
			GET_ACCESSOR(DirectionalLightOptionalParamIndexSRBuffer,		const Buffer::ShaderResourceBuffer*,	_directionalLight->optionalParamIndexBuffer.GetShaderResourceBuffer());

			GET_ACCESSOR(SpotLightTransformSRBuffer,						const Buffer::ShaderResourceBuffer*,	_spotLight->transformBuffer.GetShaderResourceBuffer());
			GET_ACCESSOR(SpotLightColorSRBuffer,							const Buffer::ShaderResourceBuffer*,	_spotLight->colorBuffer.GetShaderResourceBuffer());
			GET_ACCESSOR(SpotLightOptionalParamIndexSRBuffer,				const Buffer::ShaderResourceBuffer*,	_spotLight->optionalParamIndexBuffer.GetShaderResourceBuffer());
			GET_ACCESSOR(SpotLightParamSRBuffer,							const Buffer::ShaderResourceBuffer*,	_spotLightParamBuffer->GetShaderResourceBuffer());
		};	
	}
}
