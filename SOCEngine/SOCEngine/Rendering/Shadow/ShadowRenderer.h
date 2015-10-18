#pragma once

#include "DepthBuffer.h"
#include "VectorMap.h"

#include "PointLight.h"
#include "SpotLight.h"
#include "DirectionalLight.h"

#include "ConstBuffer.h"
#include <array>

#include "MeshCamera.h"

namespace Core
{
	class Scene;
}

namespace Rendering
{
	namespace Shadow
	{
		class ShadowRenderer
		{
		public:
			friend class Core::Scene;

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


			uint	_numOfShadowCastingPointLightInAtlas;			//default 16
			uint	_numOfShadowCastingSpotLightInAtlas;			//default 16
			uint	_numOfShadowCastingDirectionalLightInAtlas;		//default 4

			uint	_pointLightShadowMapResolution;			//default 256
			uint	_spotLightShadowMapResolution;			//default 256
			uint	_directionalLightShadowMapResolution;	//default 512

		public:
			ShadowRenderer();
			~ShadowRenderer();

		public:
			void Initialize(uint numOfShadowCastingPointLight = 16, uint numOfShadowCastingSpotLight = 16, uint numOfShadowCastingDirectionalLight = 4);

		public:
			void ResizeShadowMapAtlas(
				uint numOfShadowCastingPointLight, uint numOfShadowCastingSpotLight, uint numOfShadowCastingDirectionalLight,
				uint pointLightShadowMapResolution, uint spotLightShadowMapResolution, uint directionalLightShadowMapResolution);
			void Destroy();

		private:
			void UpdateShadowCastingSpotLightCB(const Device::DirectX*& dx, uint index);
			void UpdateShadowCastingPointLightCB(const Device::DirectX*& dx, uint index);
			void UpdateShadowCastingDirectionalLightCB(const Device::DirectX*& dx, uint index);

			void RenderSpotLightShadowMap(const Device::DirectX*& dx, const Manager::RenderManager* renderManager);
			void RenderPointLightShadowMap(const Device::DirectX*& dx, const Manager::RenderManager* renderManager);
			void RenderDirectionalLightShadowMap(const Device::DirectX*& dx, const Manager::RenderManager* renderManager);

		public:
			void AddShadowCastingLight(const Light::LightForm*& light);
			void DeleteShadowCastingLight(const Light::LightForm*& light);
			bool HasShadowCastingLight(const Light::LightForm*& light);

		private: //friend class Scene
			void UpdateShadowCastingLightCB(const Device::DirectX*& dx);
			void RenderShadowMap(const Device::DirectX*& dx, const Manager::RenderManager* renderManager);
		};
	}
}