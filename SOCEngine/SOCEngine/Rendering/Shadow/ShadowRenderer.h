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
				uint	packedNumOfShadowAtlasCapacity;

				float	pointLightTexelOffset;
				float	pointLightUnderscanScale;

				uint	packedPowerOfTwoShadowAtlasSize;
			};

		private:
			Buffer::ConstBuffer*	_shadowGlobalParamCB;
			ShadowGlobalParam		_prevShadowGlobalParam;

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

			uint	_numOfShadowCastingPointLightInAtlas;			//default 1
			uint	_numOfShadowCastingSpotLightInAtlas;			//default 1
			uint	_numOfShadowCastingDirectionalLightInAtlas;		//default 1

			uint	_pointLightShadowMapResolution;					//default 1024
			uint	_spotLightShadowMapResolution;					//default 1024
			uint	_directionalLightShadowMapResolution;			//default 2048

			float	_pointLightShadowBlurSize;						//default 4.25

		public:
			ShadowRenderer();
			~ShadowRenderer();

		public:
			void Initialize(uint numOfShadowCastingPointLight = 1, uint numOfShadowCastingSpotLight = 1, uint numOfShadowCastingDirectionalLight = 1);

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
			uint GetPackedShadowAtlasCapacity() const;
			void MakeShadowGlobalParam(ShadowGlobalParam& outParam) const;

		private: //friend class Scene
			void UpdateConstBuffer(const Device::DirectX*& dx);
			void RenderShadowMap(const Device::DirectX*& dx, const Manager::RenderManager*& renderManager);

		public:
			GET_ACCESSOR(PointLightShadowMapAtlas,			const Texture::DepthBuffer*,	_pointLightShadowMapAtlas);
			GET_ACCESSOR(SpotLightShadowMapAtlas,			const Texture::DepthBuffer*,	_spotLightShadowMapAtlas);
			GET_ACCESSOR(DirectionalLightShadowMapAtlas,	const Texture::DepthBuffer*,	_directionalLightShadowMapAtlas);

			GET_ACCESSOR(PointLightShadowMapResolution,			uint,	_pointLightShadowMapResolution);
			GET_ACCESSOR(SpotLightShadowMapResolution,			uint,	_spotLightShadowMapResolution);
			GET_ACCESSOR(DirectionalLightShadowMapResolution,	uint,	_directionalLightShadowMapResolution);

			GET_SET_ACCESSOR(PointLightShadowBlurSize, float, _pointLightShadowBlurSize);
			GET_ACCESSOR(ShadowGlobalParamConstBuffer, const Buffer::ConstBuffer*, _shadowGlobalParamCB);

			bool IsWorking() const;
		};
	}
}