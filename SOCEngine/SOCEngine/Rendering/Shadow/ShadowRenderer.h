#pragma once

#include "DepthBuffer.h"
#include "VectorMap.h"

#include "PointLight.h"
#include "SpotLight.h"
#include "DirectionalLight.h"

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
			struct ShadowCastingLight
			{
				address lightAddress;
				uint updateCounter;
			};
			Structure::VectorMap<address, ShadowCastingLight>	_shadowCastingPointLights;
			Structure::VectorMap<address, ShadowCastingLight>	_shadowCastingSpotLights;
			Structure::VectorMap<address, ShadowCastingLight>	_shadowCastingDirectionalLights;


			NumOfShadowCastingLight								_numOfShadowCastingLight;
			uint												_shadowMapResolution; //default 256

		public:
			ShadowRenderer();
			~ShadowRenderer();

		public:
			void CreateOrResizeShadowMap(const NumOfShadowCastingLight& numOfShadowCastingLight);
			void Destroy();

		private:
			Structure::VectorMap<address, ShadowCastingLight>* GetShadowCastingLights(const Light::LightForm* light);

		public:
			void AddShadowCastingLight(const Light::LightForm* light);
			void DeleteShadowCastingLight(const Light::LightForm* light);
			bool HasShadowCastingLight(const Light::LightForm* light);

		public:
			void RenderSpotLightShadowMap(const Device::DirectX* dx);
			void RenderPointLightShadowMap(const Device::DirectX* dx);
			void RenderDirectionalLightShadowMap(const Device::DirectX* dx);

			void Render(const Device::DirectX* dx);
		};
	}
}