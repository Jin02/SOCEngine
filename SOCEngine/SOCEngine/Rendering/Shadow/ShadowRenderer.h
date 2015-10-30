#pragma once

#include "DepthBuffer.h"
#include "VectorMap.h"

#include "PointLight.h"
#include "SpotLight.h"
#include "DirectionalLight.h"

#include "ConstBuffer.h"
#include <array>

#include "MeshCamera.h"
#include "BoundBox.h"

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
			struct ShadowGlobalParam
			{
				uint	packedNumOfShadowCastingLights;

				float	pointLightTexelOffset;
				float	pointLightUnderscanScale;

				float	dummy;
			};

		private:
			Texture::DepthBuffer*	_pointLightShadowMapAtlas;
			Texture::DepthBuffer*	_spotLightShadowMapAtlas;
			Texture::DepthBuffer*	_directionalLightShadowMapAtlas;

		private:
			struct LightAddress
			{
				address	lightAddress;
				Math::Matrix prevViewProjMat;
			};
			struct ShadowCastingPointLight : public LightAddress
			{
				std::array<Buffer::ConstBuffer*, 6> camConstBuffers;

			};
			struct ShadowCastingSpotDirectionalLight : public LightAddress
			{
				Buffer::ConstBuffer* camConstBuffer;
			};

		private:
			Structure::VectorMap<address, ShadowCastingPointLight>				_shadowCastingPointLights;
			Structure::VectorMap<address, ShadowCastingSpotDirectionalLight>	_shadowCastingSpotLights;
			Structure::VectorMap<address, ShadowCastingSpotDirectionalLight>	_shadowCastingDirectionalLights;

			uint	_numOfShadowCastingPointLightInAtlas;			//default 16
			uint	_numOfShadowCastingSpotLightInAtlas;			//default 16
			uint	_numOfShadowCastingDirectionalLightInAtlas;		//default 4

			uint	_pointLightShadowMapResolution;					//default 256
			uint	_spotLightShadowMapResolution;					//default 256
			uint	_directionalLightShadowMapResolution;			//default 512

			float	_pointLightShadowBlurSize;						//default 2.5f

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

			void RenderSpotLightShadowMap(const Device::DirectX*& dx, const Manager::RenderManager*& renderManager);
			void RenderPointLightShadowMap(const Device::DirectX*& dx, const Manager::RenderManager*& renderManager);
			void RenderDirectionalLightShadowMap(const Device::DirectX*& dx, const Manager::RenderManager*& renderManager);

		public:
			void AddShadowCastingLight(const Light::LightForm*& light);
			void DeleteShadowCastingLight(const Light::LightForm*& light);
			bool HasShadowCastingLight(const Light::LightForm*& light);
			ushort FetchShadowCastingLightIndex(const Light::LightForm*& light);
			uint GetPackedShadowCastingLightCount() const;
			void MakeShadowGlobalParam(ShadowGlobalParam& outParam) const;

		private: //friend class Scene
			void UpdateShadowCastingLightCB(const Device::DirectX*& dx);
			void RenderShadowMap(const Device::DirectX*& dx, const Manager::RenderManager*& renderManager);

		public:
			GET_ACCESSOR(PointLightShadowMapAtlas,			const Texture::DepthBuffer*,	_pointLightShadowMapAtlas);
			GET_ACCESSOR(SpotLightShadowMapAtlas,			const Texture::DepthBuffer*,	_spotLightShadowMapAtlas);
			GET_ACCESSOR(DirectionalLightShadowMapAtlas,	const Texture::DepthBuffer*,	_directionalLightShadowMapAtlas);

			GET_ACCESSOR(PointLightShadowMapResolution,			uint,	_pointLightShadowMapResolution);
			GET_ACCESSOR(SpotLightShadowMapResolution,			uint,	_spotLightShadowMapResolution);
			GET_ACCESSOR(DirectionalLightShadowMapResolution,	uint,	_directionalLightShadowMapResolution);

			GET_SET_ACCESSOR(PointLightShadowBlurSize, float, _pointLightShadowBlurSize);
		};
	}
}