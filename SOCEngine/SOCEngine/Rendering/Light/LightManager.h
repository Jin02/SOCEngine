#pragma once

#include "DirectionalLightBuffer.h"
#include "SpotLightBuffer.h"
#include "PointLightBuffer.h"
#include "Transform.h"

namespace Rendering
{
	namespace Manager
	{
		class LightManager final
		{
		public:
			LightManager() = default;
			DISALLOW_COPY_CONSTRUCTOR(LightManager);

			void Initialize(Device::DirectX& dx);

			template <class LightType>
			LightType& Acquire(Core::ObjectId objId)
			{
				return Add(LightType(objId));
			}

			template <class LightType>
			void Delete(Core::ObjectId objId)
			{
				auto& pool = GetPool<LightType>();
				uint index = pool.GetIndexer().Find(objId.Literal());

				GetBuffer<LightType>().Delete(index);
				pool.Delete(objId.Literal());

				uint prevDeleteIdx = GetLightDatas<LightType>().reupdateMinIndex;
				GetLightDatas<LightType>().reupdateMinIndex = min(index, prevDeleteIdx);
			}

			template <class LightType>
			bool Has(Core::ObjectId objId) const
			{
				return GetPool<LightType>().GetIndexer().Has(objId.Literal());
			}

			template <class LightType>
			auto Find(Core::ObjectId id)
			{
				return GetPool<LightType>().Find(id.Literal());
			}

			template <class LightType>
			LightType& Add(LightType& light)
			{
				light.GetBase().SetDirty(true);

				GetBuffer<LightType>().PushLight(light);

				Core::ObjectId id = light.GetObjectId();
				return GetPool<LightType>().Add(id.Literal(), light);
			}

			template <class LightType>
			void Delete(LightType& light)
			{
				Core::ObjectId id = light.GetObjectId();
				Delete<LightType>(id);

				uint prevDeleteIdx = GetLightDatas<LightType>().reupdateMinIndex;
				GetLightDatas<LightType>().reupdateMinIndex = min(index, prevDeleteIdx);
			}

			void DeleteAll();

			uint GetPackedLightCount() const;

			void UpdateTransformBuffer(const Core::TransformPool& transformPool);
			void UpdateParamBuffer(	const Light::Buffer::RequiredIndexer& indexer,
									const Core::TransformPool& transformPool );
			void UpdateSRBuffer(Device::DirectX& dx);

			void CheckDirtyLights(const Core::TransformPool& transformPool);
			void ClearDirtyLights();

			void BindResources(Device::DirectX& dx, bool bindVS, bool bindGS, bool bindPS);
			void UnbindResources(Device::DirectX& dx, bool bindVS, bool bindGS, bool bindPS) const;

			template <class LightType> auto&		GetBuffer()
			{
				return GetLightDatas<LightType>().lightBuffer;
			}
			template <class LightType> const auto&	GetBuffer() const
			{
				return GetLightDatas<LightType>().lightBuffer;
			}
			template <class LightType> Light::LightPool<LightType>&	GetPool()
			{
				return GetLightDatas<LightType>().pool;
			}
			template <class LightType> const Light::LightPool<LightType>& GetPool() const
			{
				return GetLightDatas<LightType>().pool;
			}
			template <class LightType> const auto&	GetIndexer() const
			{
				return GetPool<LightType>().GetIndexer();
			}
			template <class LightType> uint GetLightCount() const
			{
				return GetPool<LightType>().GetSize();
			}

		private:
			template <class LightType> auto&		GetLightDatas()
			{
				return std::get<LightDatas<LightType>>(_lightDatas);
			}
			template <class LightType> const auto&	GetLightDatas() const
			{
				return std::get<LightDatas<LightType>>(_lightDatas);
			}

			template <class LightType> auto&		GetDirtyParamLights()
			{
				return GetLightDatas<LightType>().dirtyParamLights;
			}
			template <class LightType> auto&		GetDirtyTransformLights()
			{
				return GetLightDatas<LightType>().dirtyTransformLights;
			}

		private:
			template <class LightType>
			struct LightDatas
			{
				Light::LightPool<LightType>			pool;
				typename LightType::LightBufferType	lightBuffer;
				std::vector<LightType*>				dirtyParamLights;
				std::vector<LightType*>				dirtyTransformLights;
				uint								reupdateMinIndex = 0;
			};

			std::tuple<	LightDatas<Light::PointLight>,
						LightDatas<Light::SpotLight>,
						LightDatas<Light::DirectionalLight>>		_lightDatas;
		};	
	}
}
