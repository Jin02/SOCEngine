#pragma once

#include "DepthBuffer.h"
#include "VectorMap.h"

#include "PointLightShadow.h"
#include "SpotLightShadow.h"
#include "DirectionalLightShadow.h"

#include "ConstBuffer.h"
#include <array>

#include "MeshCamera.h"
#include "BoundBox.h"

#include "RenderTypes.h"
#include <functional>

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
				uint packedNumOfShadowAtlasCapacity;
				uint packedPowerOfTwoShadowResolution;
				uint packedNumOfShadows;
				uint dummy;
			};
			struct ShadowMapCB
			{
				Math::Matrix viewProjMat;
			};

		private:
			Buffer::ConstBuffer*												_shadowGlobalParamCB;
			ShadowGlobalParam													_prevShadowGlobalParam;

		private:
			Texture::DepthBuffer*												_pointLightShadowMapAtlas;
			Texture::DepthBuffer*												_spotLightShadowMapAtlas;
			Texture::DepthBuffer*												_directionalLightShadowMapAtlas;

		private:
			struct PrevUpdateCounter
			{
				uint									prevParamUpateCounter;
				uint									prevTransformUpateCounter;

				Math::Matrix							prevViewProjMat;

				PrevUpdateCounter() : prevParamUpateCounter(-1), prevTransformUpateCounter(-1) {}
				~PrevUpdateCounter() {}
			};
			struct ShadowCastingPointLight : public PrevUpdateCounter
			{
				PointLightShadow*						shadow;
				std::array<Buffer::ConstBuffer*, 6>		camConstBuffers;
				ShadowCastingPointLight() : PrevUpdateCounter(), shadow(nullptr) {}
				~ShadowCastingPointLight(){}
			};
			struct ShadowCastingSpotLight : public PrevUpdateCounter
			{
				SpotLightShadow*						shadow;
				Buffer::ConstBuffer*					camConstBuffer;
				ShadowCastingSpotLight() : PrevUpdateCounter(), shadow(nullptr) {}
				~ShadowCastingSpotLight(){}
			};
			struct ShadowCastingDirectionalLight : public PrevUpdateCounter
			{
				DirectionalLightShadow*					shadow;
				Buffer::ConstBuffer*					camConstBuffer;
				ShadowCastingDirectionalLight() : PrevUpdateCounter(), shadow(nullptr) {}
				~ShadowCastingDirectionalLight(){}
			};

			Structure::VectorMap<address, ShadowCastingPointLight>				_shadowCastingPointLights;
			Structure::VectorMap<address, ShadowCastingSpotLight>				_shadowCastingSpotLights;
			Structure::VectorMap<address, ShadowCastingDirectionalLight>		_shadowCastingDirectionalLights;

		private:
			uint																_numOfShadowCastingPointLightInAtlas;			//default 1
			uint																_numOfShadowCastingSpotLightInAtlas;			//default 1
			uint																_numOfShadowCastingDirectionalLightInAtlas;		//default 1

			uint																_pointLightShadowMapResolution;					//default 1024
			uint																_spotLightShadowMapResolution;					//default 1024
			uint																_directionalLightShadowMapResolution;			//default 2048

			Structure::VectorMap<address, ShadowCommon::Param>					_pointLightShadowParamBuffer;
			Buffer::ShaderResourceBuffer*										_pointLightShadowParamSRBuffer;
			Structure::VectorHashMap<address, std::array<Math::Matrix, 6>>		_pointLightViewProjMatBuffer;
			Buffer::ShaderResourceBuffer*										_pointLightViewProjMatSRBuffer;

			Structure::VectorMap<address, ShadowCommon::Param>					_spotLightShadowParamBuffer;
			Buffer::ShaderResourceBuffer*										_spotLightShadowParamSRBuffer;
			Structure::VectorHashMap<address, Math::Matrix>						_spotLightViewProjMatBuffer;
			Buffer::ShaderResourceBuffer*										_spotLightViewProjMatSRBuffer;

			Structure::VectorMap<address, ShadowCommon::Param>					_directionalLightShadowParamBuffer;
			Buffer::ShaderResourceBuffer*										_directionalLightShadowParamSRBuffer;
			Structure::VectorHashMap<address, Math::Matrix>						_directionalLightViewProjMatBuffer;
			Buffer::ShaderResourceBuffer*										_directionalLightViewProjMatSRBuffer;

			bool																_forceUpdateDL;
			bool																_forceUpdatePL;
			bool																_forceUpdateSL;

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

			void UpdateSRBufferUsingMapDiscard(const Device::DirectX*& dx, std::function<uint(const Light::LightForm*)> getLightIndexInEachLightsFunc);
			void UpdateSRBufferUsingMapNoOverWrite(const Device::DirectX*& dx);

			void RenderSpotLightShadowMap(const Device::DirectX*& dx, const Manager::RenderManager*& renderManager);
			void RenderPointLightShadowMap(const Device::DirectX*& dx, const Manager::RenderManager*& renderManager);
			void RenderDirectionalLightShadowMap(const Device::DirectX*& dx, const Manager::RenderManager*& renderManager);

		public:
			void AddShadowCastingLight(const Light::LightForm*& light);
			bool HasShadowCastingLight(const Light::LightForm*& light);
			void DeleteShadowCastingLight(const Light::LightForm*& light);

			void MakeShadowGlobalParam(ShadowGlobalParam& outParam) const;

			uint FetchShadowIndexInEachShadowLights(const Light::LightForm* light);
			uint GetPackedShadowAtlasCapacity() const;
			void BindResources(const Device::DirectX* dx, bool bindVS, bool bindGS, bool bindPS) const;
			void UnbindResources(const Device::DirectX* dx, bool bindVS, bool bindGS, bool bindPS) const;

		private: //friend class Scene
			void UpdateConstBuffer(const Device::DirectX*& dx);

			void UpdateSRBuffer(const Device::DirectX*& dx,
								const std::function<uint(const Light::LightForm*)>& getLightIndexInEachLightsFunc);

			void RenderShadowMap(const Device::DirectX*& dx, const Manager::RenderManager*& renderManager);
			void ComputeAllLightViewProj();

		public:
			GET_ACCESSOR(PointLightShadowMapAtlas,						const Texture::DepthBuffer*,				_pointLightShadowMapAtlas);
			GET_ACCESSOR(SpotLightShadowMapAtlas,						const Texture::DepthBuffer*,				_spotLightShadowMapAtlas);
			GET_ACCESSOR(DirectionalLightShadowMapAtlas,				const Texture::DepthBuffer*,				_directionalLightShadowMapAtlas);
			GET_ACCESSOR(PointLightShadowMapResolution,					uint,										_pointLightShadowMapResolution);
			GET_ACCESSOR(SpotLightShadowMapResolution,					uint,										_spotLightShadowMapResolution);
			GET_ACCESSOR(DirectionalLightShadowMapResolution,			uint,										_directionalLightShadowMapResolution);
			GET_ACCESSOR(ShadowGlobalParamConstBuffer,					const Buffer::ConstBuffer*,					_shadowGlobalParamCB);

			GET_ACCESSOR(PointLightShadowParamSRBuffer,					const Buffer::ShaderResourceBuffer*,		_pointLightShadowParamSRBuffer);
			GET_ACCESSOR(PointLightShadowViewProjSRBuffer,				const Buffer::ShaderResourceBuffer*,		_pointLightViewProjMatSRBuffer);

			GET_ACCESSOR(SpotLightShadowParamSRBuffer,					const Buffer::ShaderResourceBuffer*,		_spotLightShadowParamSRBuffer);
			GET_ACCESSOR(SpotLightShadowViewProjSRBuffer,				const Buffer::ShaderResourceBuffer*,		_spotLightViewProjMatSRBuffer);

			GET_ACCESSOR(DirectionalLightShadowParamSRBuffer,			const Buffer::ShaderResourceBuffer*,		_directionalLightShadowParamSRBuffer);
			GET_ACCESSOR(DirectionalLightShadowViewProjSRBuffer,		const Buffer::ShaderResourceBuffer*,		_directionalLightViewProjMatSRBuffer);

			GET_ACCESSOR(DirectionalLightCount,							uint,										_shadowCastingDirectionalLights.GetSize());
			GET_ACCESSOR(PointLightCount,								uint,										_shadowCastingPointLights.GetSize());
			GET_ACCESSOR(SpotLightCount,								uint,										_shadowCastingSpotLights.GetSize());

			bool IsWorking() const;
			Math::Size<uint> GetActivatedDLShadowMapSize() const;
			Math::Size<uint> GetActivatedPLShadowMapSize() const;
			Math::Size<uint> GetActivatedSLShadowMapSize() const;
		};
	}
}