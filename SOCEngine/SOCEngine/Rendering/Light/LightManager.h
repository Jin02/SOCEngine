#pragma once

#include "DirectionalLightingBuffer.h"
#include "SpotLightingBuffer.h"
#include "PointLightingBuffer.h"
#include "Transform.h"

namespace Rendering
{
	namespace Manager
	{
		template<class LightObject>
		class LightBuffer final
		{
		public:
			void Initialize(Device::DirectX& dx)
			{
				_buffer.Initialize(dx);
			}
			void Add(typename LightObject::LightingBufferType& light, const Core::Transform& lightTF, const Light::LightingBuffer::RequiredIndexBook& indexBooks)
			{
				_buffer.AddLight(light, lightTF, indexBooks);
			}
			void Delete(typename LightObject::LightingBufferType& light)
			{
				_buffer.Delete(light);
			}
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
			void Add(LightType& light, const Core::Transform& lightTF, const Light::LightingBuffer::RequiredIndexBook& indexBooks)
			{
				GetBuffer<LightType>().Add(light, lightTF, indexBooks);
				GetPool<LightType>().Add(light.GetLightId().Literal(), light);
			}

			template <class LightType>
			void Delete(LightType& light)
			{
				GetBuffer<LightType>().Delete(light);
				GetPool<LightType>().Delete(light.GetLightId().Literal());
			}

			uint GetPackedLightCount() const;

			void UpdateTransformBuffer(const std::vector<Core::Transform*>& dirtyTransforms);
			void UpdateParamBuffer(	const Light::LightingBuffer::RequiredIndexBook& indexBooks,
									const Core::TransformPool& transformPool	);
			void UpdateSRBuffer(Device::DirectX& dx);

			void CheckDirtyLights();
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

		private:
			template <class LightType> auto&		GetDirtyLights()
			{
				return std::get<std::vector<LightType*>>(_dirtyLigts);
			}
			template <class LightType> const auto&	GetDirtyLights() const
			{
				return std::get<std::vector<LightType*>>(_dirtyLigts);
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
			> _dirtyLigts;
		};	
	}
}
