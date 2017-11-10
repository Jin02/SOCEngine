#pragma once

#include "DirectionalLightBuffer.h"
#include "SpotLightBuffer.h"
#include "PointLightBuffer.h"
#include "Transform.h"

namespace Rendering
{
	namespace Manager
	{
		class ShadowManager;
		class LightManager final
		{
		public:
			LightManager() = default;
			DISALLOW_COPY_CONSTRUCTOR(LightManager);

			void Initialize(Device::DirectX& dx);

			template <class LightType>
			LightType& Acquire(Core::ObjectID objID)
			{
				return Add(LightType(objID));
			}

			template <class LightType>
			void Delete(Core::ObjectID objID)
			{
				auto& pool = GetPool<LightType>();
				uint index = pool.GetIndexer().Find(objID.Literal());

				GetBuffer<LightType>().Delete(index);
				pool.Delete(objID.Literal());

				GetLightDatas<LightType>().mustUpdateToGPU = true;
				_changedLightCounts	= true;
			}

			template <class LightType>
			bool Has(Core::ObjectID objID) const
			{
				return GetPool<LightType>().Has(objID.Literal());
			}

			template <class LightType>
			auto Find(Core::ObjectID id)
			{
				return GetPool<LightType>().Find(id.Literal());
			}

			template <class LightType>
			const auto Find(Core::ObjectID id) const
			{
				return GetPool<LightType>().Find(id.Literal());
			}


			template <class LightType>
			LightType& Add(LightType& light)
			{
				light.GetBase().SetDirty(true);
				_changedLightCounts = true;

				GetBuffer<LightType>().PushLight(light);

				Core::ObjectID id = light.GetObjectID();
				return GetPool<LightType>().Add(id.Literal(), light);
			}

			template <class LightType>
			void Delete(LightType& light)
			{
				Core::ObjectID id = light.GetObjectID();
				Delete<LightType>(id);
				_changedLightCounts = true;
			}

			void DeleteAll();

			uint GetPackedLightCount() const;

			void UpdateTransformBuffer(const Core::TransformPool& transformPool);
			void UpdateParamBuffer(const ShadowManager& shadowMgr, const Core::ObjectID::IndexHashMap& shaftIndexer );
			void UpdateSRBuffer(Device::DirectX& dx);

			void CheckDirtyLights(const Core::TransformPool& transformPool);
			void ClearDirty();

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

			GET_CONST_ACCESSOR(ChangedLightCount, bool, _changedLightCounts);

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

				bool								mustUpdateToGPU = true;
			};

			std::tuple<	LightDatas<Light::PointLight>,
						LightDatas<Light::SpotLight>,
						LightDatas<Light::DirectionalLight>>		_lightDatas;

			bool													_changedLightCounts = true;
		};	
	}
}
