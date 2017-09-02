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
			void ClearDirty();

			void UpdateBuffer(const LightManager& lightMgr, const Core::TransformPool& tfPool, const Intersection::BoundBox& sceneBoundBox);
			void UpdateSRBuffer(Device::DirectX& dx);
			void UpdateConstBuffer(Device::DirectX& dx);

			template <class ShadowType>
			ShadowType& Acquire(Core::ObjectID objID)
			{
				return Add( ShadowType(objID) );
			}

			template <class ShadowType>	ShadowType& Add(ShadowType& shadow)
			{
				GetBuffer<ShadowType>().GetBuffer().PushShadow(shadow);
				GetShadowMapCB<ShadowType>().PushConstBufferToQueue();

				shadow.SetDirty(true);
				_dirtyGlobalParam = true;

				uint objLiteralID = shadow.GetObjectID().Literal();
				return GetPool<ShadowType>().Add(objLiteralID, shadow);
			}
			template <class ShadowType>	void Delete(Core::ObjectID objID)
			{			
				uint index = GetPool<ShadowType>().GetIndexer().Find(objID.Literal());

				GetPool<ShadowType>().Delete(objID.Literal());
				GetBuffer<ShadowType>().GetBuffer().Delete(index);
				GetShadowMapCB<ShadowType>().Delete(index);

				_dirtyGlobalParam = true;
			}
			template <class ShadowType>	bool Has(Core::ObjectID objID) const
			{
				return GetPool<ShadowType>().Has(objID.Literal());
			}
			template <class ShadowType>	auto Find(Core::ObjectID id)
			{
				return GetPool<ShadowType>().Find(id.Literal());
			}

			void DeleteAll();

			void BindResources(Device::DirectX& dx, bool bindVS, bool bindGS, bool bindPS);
			void UnbindResources(Device::DirectX& dx, bool bindVS, bool bindGS, bool bindPS) const;

		public:
			template <class ShadowType>	auto& GetPool()
			{
				return GetShadowDatas<ShadowType>().pool;
			}
			template <class ShadowType>	const auto& GetPool() const
			{
				return GetShadowDatas<ShadowType>().pool;
			}
			template <class ShadowType>	const auto& GetIndexer() const
			{
				return GetPool<ShadowType>().GetIndexer();
			}

			template <class ShadowType>	auto& GetShadowMapCB()
			{
				return GetShadowDatas<ShadowType>().constBuffers;
			}
			template <class ShadowType> const auto& GetShadowMapCB() const
			{
				return GetShadowDatas<ShadowType>().constBuffers;
			}
			template <class ShadowType>	auto& GetBuffer()
			{
				return GetShadowDatas<ShadowType>().buffers;
			}
			template <class ShadowType>	const auto& GetBuffer() const
			{
				return GetShadowDatas<ShadowType>().buffers;
			}

			GET_ACCESSOR(GlobalParamCB, auto&, _globalCB);

		private:
			template <class ShadowType> auto&		GetShadowDatas()
			{
				return std::get<ShadowDatas<ShadowType>>(_datas);
			}
			template <class ShadowType> const auto&	GetShadowDatas() const
			{
				return std::get<ShadowDatas<ShadowType>>(_datas);
			}

			template <class ShadowType>	auto& GetDirtyShadows()
			{
				return GetShadowDatas<ShadowType>().dirtyShadows;
			}
			template <class ShadowType>	const auto& GetDirtyShadows() const
			{
				return GetShadowDatas<ShadowType>().dirtyShadows;
			}

		private:
			template <typename ShadowType>
			struct ShadowDatas
			{
				Shadow::ShadowPool<ShadowType>					pool;
				std::vector<ShadowType*>						dirtyShadows;
				Shadow::Buffer::ShadowBufferObject<ShadowType>	buffers;
				Shadow::Buffer::ShadowMapCB<ShadowType>			constBuffers;
			};

			std::tuple<	ShadowDatas<Shadow::SpotLightShadow>,
						ShadowDatas<Shadow::PointLightShadow>,
						ShadowDatas<Shadow::DirectionalLightShadow>	> _datas;

			GlobalParamCB		_globalCB;
			bool				_dirtyGlobalParam = true;
		};
	}
}
