#pragma once

#include <vector>
#include "ConstBuffer.h"

#include "DirectionalLightShadowBuffer.h"
#include "SpotLightShadowBuffer.h"
#include "PointLightShadowBuffer.h"

#include "LightManager.h"
#include "ShadowMapCBPool.h"
#include "PointLightShadowMapCBPool.h"

#include "CameraManager.h"
#include "ShadowGlobalParamCB.h"

namespace Rendering
{
	namespace Renderer
	{
		class ShadowAtlasMapRenderer;
	}

	namespace Manager
	{
		class ShadowManager final
		{
		public:
			void Initialize(Device::DirectX& dx);

			void CheckDirtyWithCullShadows(const Manager::CameraManager& camMgr, const Core::ObjectManager& objMgr, const LightManager& lightMgr, const Core::TransformPool& tfPool);			
			void ClearDirty();

			void UpdateBuffer(const LightManager& lightMgr, const Core::TransformPool& tfPool, const Intersection::BoundBox& sceneBoundBox);
			void UpdateSRBuffer(Device::DirectX& dx);
			void UpdateConstBuffer(Device::DirectX& dx, const Renderer::ShadowAtlasMapRenderer& renderer);

			template <class ShadowType>
			ShadowType& Acquire(Core::ObjectID objID)
			{
				return Add( ShadowType(objID) );
			}

			template <class ShadowType>	ShadowType& Add(ShadowType& shadow)
			{
				_changedShadowCounts = true;

				GetBuffer<ShadowType>().PushShadow(shadow);
				GetShadowMapVPMatCBPool<ShadowType>().PushConstBufferToQueue();

				uint objLiteralID = shadow.GetObjectID().Literal();
				return GetPool<ShadowType>().Add(objLiteralID, shadow);
			}
			template <class ShadowType>	void Delete(Core::ObjectID objID)
			{			
				uint index = GetPool<ShadowType>().GetIndexer().Find(objID.Literal());

				GetPool<ShadowType>().Delete(objID.Literal());
				GetBuffer<ShadowType>().Delete(index);
				GetShadowMapVPMatCBPool<ShadowType>().Delete(index);

				GetShadowDatas<ShadowType>().mustUpdateToGPU = true;
				_changedShadowCounts = true;
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

			template <class ShadowType>	auto& GetShadowMapVPMatCBPool()
			{
				return GetShadowDatas<ShadowType>().cbPool;
			}
			template <class ShadowType> const auto& GetShadowMapVPMatCBPool() const
			{
				return GetShadowDatas<ShadowType>().cbPool;
			}
			template <class ShadowType>	auto& GetBuffer()
			{
				return GetShadowDatas<ShadowType>().buffers;
			}
			template <class ShadowType>	const auto& GetBuffer() const
			{
				return GetShadowDatas<ShadowType>().buffers;
			}

			template <class ShadowType>	auto& GetInfluentialLights()
			{
				return GetShadowDatas<ShadowType>().influentialLights;
			}
			template <class ShadowType>	const auto& GetInfluentialLights() const
			{
				return GetShadowDatas<ShadowType>().influentialLights;
			}

			GET_CONST_ACCESSOR(GlobalCB,				const auto&,	_globalCB);
			GET_CONST_ACCESSOR(HasChangedShadowCount,	bool,			_changedShadowCounts);

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
				Shadow::ShadowPool<ShadowType>							pool;
				typename ShadowType::ShadowBufferType					buffers;
				typename ShadowType::CBPoolType							cbPool;

				std::vector<ShadowType*>								dirtyShadows;
				std::vector<const typename ShadowType::LightType*>		influentialLights;

				bool													mustUpdateToGPU = true;
			};

			std::tuple<	ShadowDatas<Shadow::SpotLightShadow>,
						ShadowDatas<Shadow::PointLightShadow>,
						ShadowDatas<Shadow::DirectionalLightShadow>	> _datas;

			Shadow::Buffer::ShadowGlobalParamCB							_globalCB;
			bool														_changedShadowCounts = true;

		};
	}
}
