#pragma once

#include "DepthBuffer.h"
#include "VectorMap.h"

#include "PointLight.h"
#include "SpotLight.h"
#include "DirectionalLight.h"

#include "ConstBuffer.h"
#include <array>

#include "CameraForm.h"

namespace Rendering
{
	namespace Shadow
	{
		class ShadowRenderer
		{
		public:
			struct NumOfShadowCastingLight
			{
				uint pointLight;
				uint spotLight;
				uint directionalLight;

				NumOfShadowCastingLight() 
					: pointLight(16), spotLight(16), directionalLight(4) {}
				~NumOfShadowCastingLight(){}
			};

		private:
			Texture::DepthBuffer*	_pointLightShadowMap;
			Texture::DepthBuffer*	_spotLightShadowMap;
			Texture::DepthBuffer*	_directionalLightShadowMap;

		private:
			struct LightAddress
			{
				address	lightAddress;
				Camera::CameraForm::CamConstBufferData prevConstBufferData;
			};

			struct ShadowCastingPointLight : public LightAddress
			{
				std::array<Buffer::ConstBuffer*, 6> camConstBuffers;

			};
			Structure::VectorMap<address, ShadowCastingPointLight>				_shadowCastingPointLights;

			struct ShadowCastingSpotDirectionalLight : public LightAddress
			{
				Buffer::ConstBuffer* camConstBuffer;
			};
			Structure::VectorMap<address, ShadowCastingSpotDirectionalLight>	_shadowCastingSpotLights;
			Structure::VectorMap<address, ShadowCastingSpotDirectionalLight>	_shadowCastingDirectionalLights;


			NumOfShadowCastingLight								_numOfShadowCastingLight;
			uint												_shadowMapResolution; //default 256

		public:
			ShadowRenderer();
			~ShadowRenderer();

		private:
			void UpdateShadowCastingSpotLight(const Device::DirectX*& dx, uint index);
			void UpdateShadowCastingPointLight(const Device::DirectX*& dx, uint index);

		public:
			void CreateOrResizeShadowMap(const NumOfShadowCastingLight& numOfShadowCastingLight);
			void Destroy();

		public:
			void AddShadowCastingLight(const Light::LightForm*& light);
			void DeleteShadowCastingLight(const Light::LightForm*& light);
			bool HasShadowCastingLight(const Light::LightForm*& light);

		public:
			void UpdateConstBuffer(const Device::DirectX*& dx);

			void RenderSpotLightShadowMap(const Device::DirectX*& dx);
			void RenderPointLightShadowMap(const Device::DirectX*& dx);
			void RenderDirectionalLightShadowMap(const Device::DirectX*& dx);

			void Render(const Device::DirectX*& dx);
		};
	}
}