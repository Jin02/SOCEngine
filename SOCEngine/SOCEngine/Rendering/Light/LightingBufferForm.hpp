#pragma once

#include "GPUUploadBuffer.hpp"
#include "CommonLightingBuffer.h"
#include "Transform.h"
#include <assert.h>

namespace Rendering
{
	namespace Light
	{
		namespace LightingBuffer
		{
			struct RequiredIndexBook
			{
				const Core::IndexHashMap<LightId::LiteralType>& shadowIndexBook;
				const Core::IndexHashMap<LightId::LiteralType>& lightShaftIndexBook;
			};

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
					TransformType dummy[2048];
					memset(dummy, 0, sizeof(TransformType) * 2048);
					_transformBuffer.Initialize(dx, maxLightCount, format, dummy);

					_commonBuffer.Initialize(dx, maxLightCount, dummy);
				}

				void AddLight(const LightType& light, const Core::Transform& lightTransform, const RequiredIndexBook& indexBooks)
				{
					assert(light.GetObjectId() == lightTransform.GetObjectId());

					uint lightId = light.GetBase().GetLightId().Literal();
					ushort shadowIdx = indexBooks.shadowIndexBook.Find(lightId);
					uint lightShaftIdx = indexBooks.lightShaftIndexBook.Find(lightId);

					_transformBuffer.AddData(lightId, light.MakeTransform(lightTransform));
					_commonBuffer.AddData(light.GetBase(), shadowIdx, lightShaftIdx);
				}

				void UpdateTransformBuffer(	const std::vector<Core::Transform*>& dirtyTransform,
											const LightPool<LightType>& lightPool)
				{
					for (const auto& tf : dirtyTransform)
					{
						Core::ObjectId objId = tf->GetObjectId();
						const auto light = lightPool.Find(objId.Literal());
						
						uint lightId = light->GetBase().GetLightId().Literal();
						_transformBuffer.SetData(lightId, light->MakeTransform(*tf));
					}

					_mustUpdateTransformSRBuffer |= (dirtyTransform.empty() != false);
				}

				void UpdateLightCommonBuffer(const std::vector<LightType*>& dirtyLights, RequiredIndexBook indexBooks)
				{
					for (auto& light : dirtyLights)
					{
						uint lightId = light->GetLightId().Literal();

						ushort shadowIdx = indexBooks.shadowIndexBook.Find(lightId);
						uint lightShaftIdx = indexBooks.lightShaftIndexBook.Find(lightId);

						_commonBuffer.SetData(light->GetBase(), shadowIdx, lightShaftIdx);
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
					LightId id = light.GetLightId();
					_transformBuffer.Delete(id.Literal());
					_commonBuffer.Delete(id);
				}
				
				void DeleteAll()
				{
					_transformBuffer.DeleteAll();
					_commonBuffer.DeleteAll();
				}
				
			public:
				GET_ACCESSOR(TransformSRBuffer,				auto&, _transformBuffer.GetShaderResourceBuffer());
				GET_ACCESSOR(ColorSRBuffer,					auto&, _commonBuffer.GetColorSRBuffer());
				GET_ACCESSOR(OptionalParamIndexSRBuffer, 	auto&, _commonBuffer.GetOptionalParamIndexSRBuffer());
				GET_CONST_ACCESSOR(IndexBook,				const auto&, _transformBuffer.IndexBook());

				GET_CONST_ACCESSOR(Size, uint, _transformBuffer.GetSize());

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