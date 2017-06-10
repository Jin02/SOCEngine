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
				using TransformBuffer			= Buffer::GPUUploadBuffer<TransformType>;
				
			public:
				LightingBufferForm() = default;
				
				void Initialize(Device::DirectX& dx, uint maxLightCount, DXGI_FORMAT format)
				{
					const TransformType dummy[2048] = { 0, };
					_transformBuffer.Initialize(dx, maxLightCount, format, dummy);

					_commonBuffer.Initialize(dx, maxLightCount, dummy);
				}

				void RegistLight(const LightType& light,
					const Core::IndexHashMap<LightId::LiteralType>& shadowIndexBook,
					const Core::IndexHashMap<LightId::LiteralType>& lightShaftIndexBook)
				{
					uint lightId = light->GetBase().GetLightId().Literal();
					ushort shadowIdx = shadowIndexBook.Find(lightId);
					uint lightShaftIdx = lightShaftIndexBook.Find(lightId);

					_transformBuffer.RegistData(lightId, light->MakeTransform(tfBufferElem, tf));
					_commonBuffer.RegistData(light->GetBase(), shadowIdx, lightShaftIdx)
				}

				void UpdateTransformBuffer(	const std::vector<Core::Transform*>& dirtyTransform,
											const LightPool<LightType>& lightPool)
				{
					for (auto& tf : dirtyTransform)
					{
						Core::ObjectId objId = tf->GetObjectId();
						const LightType& light = lightPool.Find(objId);
						
						uint lightId = light->GetBase().GetLightId().Literal();
						_transformBuffer.SetData(lightId, light->MakeTransform(tfBufferElem, tf));
					}

					_mustUpdateTransformSRBuffer |= (dirtyTransform.empty() != false);
				}

				void UpdateLightCommonBuffer(const std::vector<LightType*>& dirtyLights,
											const Core::IndexHashMap<LightId::LiteralType>& shadowIndexBook,
											const Core::IndexHashMap<LightId::LiteralType>& lightShaftIndexBook)
				{
					for (auto& light : dirtyLights)
					{
						uint lightId = light->GetLightId().Literal();

						ushort shadowIdx = shadowIndexBook.Find(lightId);
						uint lightShaftIdx = lightShaftIndexBook.Find(lightId);

						_commonBuffer.SetBufferData(light->GetBase(), shadowIdx, lightShaftIdx);
					}

					_mustUpdateCommonSRBuffer |= (dirtyLights.empty() != false);
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

			protected:
				TransformBuffer					_transformBuffer;
				CommonLightingBuffer			_commonBuffer;

			private:
				bool							_mustUpdateCommonSRBuffer		= true;
				bool							_mustUpdateTransformSRBuffer	= true;
			};
		}
	}
}