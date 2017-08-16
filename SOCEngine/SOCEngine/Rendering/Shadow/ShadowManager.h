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
			ShadowType& Acquire(Core::ObjectId objId)
			{
				return Add( ShadowType(objId) );
			}

			template <class ShadowType>	ShadowType& Add(ShadowType& shadow)
			{
				GetBuffer<ShadowType>().GetBuffer().PushShadow(shadow);
				GetShadowMapCB<ShadowType>().PushConstBufferToQueue();

				shadow.SetDirty(true);
				_dirtyGlobalParam = true;

				uint objLiteralId = shadow.GetObjectId().Literal();
				return GetPool<ShadowType>().Add(objLiteralId, shadow);
			}
			template <class ShadowType>	void Delete(Core::ObjectId objId)
			{			
				uint index = GetPool<ShadowType>().GetIndexer().Find(objId.Literal());

				GetPool<ShadowType>().Delete(objId.Literal());
				GetBuffer<ShadowType>().GetBuffer().Delete(index);
				GetShadowMapCB<ShadowType>().Delete(index);

				uint prevDeleteIdx = GetShadowDatas<ShadowType>().reupdateMinIndex;
				GetShadowDatas<ShadowType>().reupdateMinIndex = min(index, prevDeleteIdx);

				_dirtyGlobalParam = true;
			}
			template <class ShadowType>	bool Has(Core::ObjectId objId) const
			{
				return GetPool<ShadowType>().GetIndexer().Has(objId.Literal());
			}
			template <class ShadowType>	auto Find(Core::ObjectId id)
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
				uint											reupdateMinIndex = 0;
			};

			std::tuple<	ShadowDatas<Shadow::SpotLightShadow>,
						ShadowDatas<Shadow::PointLightShadow>,
						ShadowDatas<Shadow::DirectionalLightShadow>	> _datas;

			GlobalParamCB		_globalCB;
			bool				_dirtyGlobalParam = true;
		};
	}
}