#pragma once

#include <vector>
#include "ConstBuffer.h"

#include "DirectionalLightShadowBuffer.h"
#include "SpotLightShadowBuffer.h"
#include "PointLightShadowBuffer.h"

#include "LightManager.h"
#include "ShadowMapCB.hpp"
#include "ShadowBufferObject.hpp"

namespace Rendering
{
	namespace Manager
	{
		class ShadowManager final
		{
		public:
			struct ShadowGlobalParam
			{
				uint packedNumOfShadowAtlasCapacity;
				uint packedPowerOfTwoShadowResolution;
				uint packedNumOfShadows;
				uint dummy;
			};
			using GlobalParamCB = Rendering::Buffer::ExplicitConstBuffer<ShadowGlobalParam>;

			constexpr static const uint DirectionalLightInitCount = 1;
			constexpr static const uint SpotLightInitCount = 1;
			constexpr static const uint PointLightInitCount = 1;

		public:
			void Initialize(Device::DirectX& dx);
			void UpdateGlobalCB(Device::DirectX& dx);

			void CheckDirtyShadows(const LightManager& lightMgr, const Core::TransformPool& tfPool);			
			void ClearDirtyShadows();

			void UpdateNotInitedCB(Device::DirectX& dx);
			void UpdateBuffer(const LightManager& lightMgr, const Core::TransformPool& tfPool, const Intersection::BoundBox& sceneBoundBox);
			void UpdateSRBuffer(Device::DirectX& dx);

			template <class ShadowType>
			ShadowType& Acquire(Light::LightId lightId)
			{
				Shadow::ShadowId id = _idMgr.Acquire();
				return Add( ShadowType(lightId, id) );
			}

			template <class ShadowType>
			ShadowType& Add(ShadowType& shadow)
			{
				uint id = shadow.GetShadowId().Literal();
				GetBuffer<ShadowType>().GetBuffer().AddShadow(shadow);
				GetShadowMapCB<ShadowType::LightType>().Add(shadow.GetShadowId());

				shadow.SetDirty(true);
				_dirtyGlobalParam = true;

				return GetPool<ShadowType::LightType>().Add(id, shadow);
			}
			template <class ShadowType>
			void Delete(ShadowType& shadow)
			{
				uint id = shadow.GetShadowId().Literal();

				GetPool<ShadowType::LightType>().Delete(id);
				GetBuffer<ShadowType>().GetBuffer().Delete(shadow);
				GetShadowMapCB<ShadowType::LightType>().Delete(shadow.GetShadowId());

				_dirtyGlobalParam = true;
				_idMgr.Delete(shadow.GetShadowId());
			}
			void DeleteAll();

			void BindResources(Device::DirectX& dx, bool bindVS, bool bindGS, bool bindPS);
			void UnbindResources(Device::DirectX& dx, bool bindVS, bool bindGS, bool bindPS) const;

		public:
			template <class LightType>	auto& GetPool()
			{
				return std::get<Shadow::ShadowPool<LightType>>(_shadowPool);
			}
			template <class LightType>	const auto& GetPool() const
			{
				return std::get<Shadow::ShadowPool<LightType>>(_shadowPool);
			}
			template <class LightType>	const auto& GetIndexBook() const
			{
				return GetPool<LightType>().GetIndexer();
			}
			template <class LightType>	auto& GetShadowMapCB()
			{
				return std::get<Shadow::Buffer::ShadowMapCB<LightType::ShadowType>>(_shadowMapCBs);
			}
			template <class ShadowType>	auto& GetBuffer()
			{
				return std::get<Shadow::Buffer::ShadowBufferObject<ShadowType>>(_shadowBuffers);
			}
			template <class ShadowType>	const auto& GetBuffer() const
			{
				return std::get<Shadow::Buffer::ShadowBufferObject<ShadowType>>(_shadowBuffers);
			}
			GET_ACCESSOR(GlobalParamCB, auto&, _globalCB);

		private:
			template <class ShadowType>	auto& GetDirtyShadows()
			{
				return std::get<std::vector<ShadowType*>>(_dirtyShadows);
			}

		private:
			std::tuple<
				Shadow::Buffer::ShadowBufferObject<Shadow::SpotLightShadow>,
				Shadow::Buffer::ShadowBufferObject<Shadow::PointLightShadow>,
				Shadow::Buffer::ShadowBufferObject<Shadow::DirectionalLightShadow>
			> _shadowBuffers;
			std::tuple<
				std::vector<Shadow::SpotLightShadow*>,
				std::vector<Shadow::PointLightShadow*>,
				std::vector<Shadow::DirectionalLightShadow*>
			> _dirtyShadows;
			std::tuple<
				Shadow::ShadowPool<Light::SpotLight>,
				Shadow::ShadowPool<Light::PointLight>,
				Shadow::ShadowPool<Light::DirectionalLight>
			> _shadowPool;
			std::tuple<
				Shadow::Buffer::ShadowMapCB<Shadow::SpotLightShadow>,
				Shadow::Buffer::ShadowMapCB<Shadow::PointLightShadow>,
				Shadow::Buffer::ShadowMapCB<Shadow::DirectionalLightShadow>
			> _shadowMapCBs;

			GlobalParamCB _globalCB;
			bool _dirtyGlobalParam = true;

			Shadow::ShadowIdManager _idMgr;
		};
	}
}