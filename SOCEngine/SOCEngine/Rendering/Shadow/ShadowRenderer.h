#pragma once

#include "DepthBuffer.h"
#include "LightForm.h"


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

			NumOfShadowCastingLight		_numOfShadowCastingLight;
			uint						_shadowMapResolution; //default 256

		public:
			ShadowRenderer();
			~ShadowRenderer();

		public:
			void CreateOrResizeShadowMap(const NumOfShadowCastingLight& numOfShadowCastingLight);
			void Destroy();

		public:

		public:
			void Render(const Device::DirectX* dx);
		};
	}
}