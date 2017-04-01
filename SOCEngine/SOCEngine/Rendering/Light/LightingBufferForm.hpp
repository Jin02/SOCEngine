#pragma once

#include "GPUUploadBuffer.hpp"
#include "CommonLightingBuffer.h"
#include "Transform.h"
#include <functional>

namespace Rendering
{
	namespace Light
	{
		namespace LightingBuffer
		{
			template<typename LightType>
			class LightingBufferForm
			{
			public:
				using TransformType				= typename LightType::TransformType;
				using TransformBuffer			= Buffer::GPUUploadBuffer<Core::ObjectId::LiteralType, TransformType>;
				
			public:
				LightingBufferForm() = default;
				
				void Initialize(Device::DirectX& dx, uint maxLightCount, DXGI_FORMAT format)
				{
					const TransformType dummy[2048] = { 0, };
					_transformBuffer.Initialize(dx, maxLightCount, format, dummy);

					_commonBuffer.Initialize(dx, maxLightCount);
				}
				void Destroy()
				{
					_commonBuffer.Destroy();
					_transformBuffer.Destroy();
				}

			public:
				void UpdateBuffer(std::vector<LightType>& lights,
						  const std::function<uchar(const BaseLight&)>& getShadowIndex,
						  const std::function<uchar(const BaseLight&)>& getLightShaftIndex,
					const Core::TransformPool& pool,
					const std::function<void(LightType&, const Core::Transform&, bool existElem)>* updateParamBuffer = nullptr)
				{
					
					for (auto& light : lights)
					{
						Core::ObjectId key = light.GetObjectId();
						const Core::Transform& transform = pool.Get( pool.GetIndexer().Find(key) );

						auto existTarnsform = _transformBuffer.Find(key);
						if (transform.GetDirty())
						{
							TransformType tfBufferElem;
							light.MakeTransform(tfBufferElem, transform);

							if (existTarnsform == nullptr)	_transformBuffer.Add(key, tfBufferElem);
							else							(*existTarnsform) = tfBufferElem;

							_mustUpdateTransformSRBuffer = true;
						}
				
						if (light.GetBase().GetDirty())
						{
							_commonBuffer.UpdateBuffer(light.GetBase(), getShadowIndex, getLightShaftIndex, existTarnsform != nullptr);
							_mustUpdateCommonSRBuffer = true;

							light.GetBase().SetDirty(false);
						}

						if(updateParamBuffer)
							(*updateParamBuffer)(light, transform, existTarnsform != nullptr);
					}
				}
				
				void UpdateSRBuffer(Device::DirectX& dx)
				{
					if(_mustUpdateTransformSRBuffer)		_transformBuffer.UpdateSRBuffer(dx);
					if(_mustUpdateCommonSRBuffer)			_commonBuffer.UpdateSRBuffer(dx);

					_mustUpdateCommonSRBuffer =
					_mustUpdateTransformSRBuffer = false;
				}
				
				void Delete(const LightType& light)
				{
					Core::ObjectId id = light.GetObjectId();
					_transformBuffer.Delete(id);
					_commonBuffer.Delete(id);
				}
				
				void DeleteAll()
				{
					_transformBuffer.DeleteAll();
					_commonBuffer.DeleteAll();
				}
				
			public:
				GET_CONST_ACCESSOR(TransformSRBuffer,			const Buffer::ShaderResourceBuffer&, _transformBuffer.GetShaderResourceBuffer());
				GET_CONST_ACCESSOR(ColorSRBuffer,				const Buffer::ShaderResourceBuffer&, _commonBuffer.GetColorSRBuffer());
				GET_CONST_ACCESSOR(OptionalParamIndexSRBuffer, 	const Buffer::ShaderResourceBuffer&, _commonBuffer.GetOptionalParamIndexSRBuffer());

			private:
				TransformBuffer					_transformBuffer;
				CommonLightingBuffer			_commonBuffer;
				bool							_mustUpdateCommonSRBuffer		= true;
				bool							_mustUpdateTransformSRBuffer	= true;
			};
		}
	}
}