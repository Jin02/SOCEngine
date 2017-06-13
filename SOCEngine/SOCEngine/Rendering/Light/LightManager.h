#pragma once

#include "DirectionalLightingBuffer.h"
#include "SpotLightingBuffer.h"
#include "PointLightingBuffer.h"
#include "Transform.h"

#include "LightId.hpp"

namespace Rendering
{
	namespace Manager
	{
		template<class LightObject>
		class LightBuffer final
		{
		public:
			auto& GetLightingBuffer()
			{
				return _buffer;
			}
			const auto& GetLightingBuffer() const
			{
				return _buffer;
			}

			GET_CONST_ACCESSOR(Size, uint, _buffer.GetSize());

		private:
			typename LightObject::LightingBufferType _buffer;
		};

		class LightManager final
		{
		public:
			DISALLOW_COPY_CONSTRUCTOR(LightManager);

			void Initialize(Device::DirectX& dx);

			template <class LightType>
			LightType& Acquire(Core::ObjectId objId)
			{
				Light::LightId id = _idMgr.Acquire();
				return Add(LightType(objId, id) );
			}

			template <class LightType>
			LightType& Add(LightType& light)
			{
				light.GetBase().SetDirty(true);

				GetBuffer<LightType>().Add(light);
				return GetPool<LightType>().Add(light.GetLightId().Literal(), light);
			}

			template <class LightType>
			void Delete(LightType& light)
			{
				GetBuffer<LightType>().Delete(light);
				GetPool<LightType>().Delete(light.GetLightId().Literal());

				_idMgr.Delete(light.GetLightId());
			}
			void DeleteAll();

			uint GetPackedLightCount() const;

			void UpdateTransformBuffer(const Core::TransformPool& transformPool);
			void UpdateParamBuffer(	const Light::LightingBuffer::RequiredIndexBook& indexBooks,
									const Core::TransformPool& transformPool );
			void UpdateSRBuffer(Device::DirectX& dx);

			void CheckDirtyLights(const Core::TransformPool& transformPool);
			void ClearDirtyLights();

			void BindResources(Device::DirectX& dx, bool bindVS, bool bindGS, bool bindPS);
			void UnbindResources(Device::DirectX& dx, bool bindVS, bool bindGS, bool bindPS) const;

			template <class LightType> auto&		GetBuffer()
			{
				return std::get<LightBuffer<LightType>>(_lightBuffers);
			}
			template <class LightType> const auto&	GetBuffer() const
			{
				return std::get<LightBuffer<LightType>>(_lightBuffers);
			}
			template <class LightType> auto&		GetPool()
			{
				return std::get<Light::LightPool<LightType>>(_lightPools);
			}
			template <class LightType> const auto&	GetPool() const
			{
				return std::get<Light::LightPool<LightType>>(_lightPools);
			}
			template <class LightType> const auto& GetIndexBook() const
			{
				return GetPool<LightType>().GetIndexer();
			}

		private:
			template <class LightType> auto&		GetDirtyParamLights()
			{
				return std::get<std::vector<LightType*>>(_dirtyParamLigts);
			}
			template <class LightType> auto&		GetDirtyTFLights()
			{
				return std::get<std::vector<LightType*>>(_dirtyParamLigts);
			}

		private:
			std::tuple<
					LightBuffer<Light::DirectionalLight>,
					LightBuffer<Light::PointLight>,
					LightBuffer<Light::SpotLight>
			> _lightBuffers;
			std::tuple<				
				Light::LightPool<Light::DirectionalLight>,
				Light::LightPool<Light::PointLight>,
				Light::LightPool<Light::SpotLight>
			> _lightPools;

			std::tuple<
				std::vector<Light::DirectionalLight*>,
				std::vector<Light::PointLight*>,
				std::vector<Light::SpotLight*>
			> _dirtyParamLigts;

			std::tuple<
				std::vector<Light::DirectionalLight*>,
				std::vector<Light::PointLight*>,
				std::vector<Light::SpotLight*>
			> _dirtyTFLigts;

			Light::LightIdManager _idMgr;
		};	
	}
}
