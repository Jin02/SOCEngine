#pragma once

#include "GPUUploadBuffer.hpp"
#include "CommonLightBuffer.h"
#include "Transform.h"
#include <assert.h>

namespace Rendering
{
	namespace Light
	{
		namespace Buffer
		{
			struct RequiredIndexBook
			{
				const Core::IndexHashMap<LightId::LiteralType>& shadowIndexBook;
				const Core::IndexHashMap<LightId::LiteralType>& lightShaftIndexBook;
			};

			template<typename LightType>
			class LightBufferForm
			{
			public:
				using TransformType				= typename LightType::TransformType;
				using TransformBuffer			= Rendering::Buffer::GPUUploadBuffer<TransformType>;
				
			public:
				LightBufferForm() = default;
				
				void Initialize(Device::DirectX& dx, uint maxLightCount, DXGI_FORMAT tfFormat)
				{
					TransformType dummy[POINT_LIGHT_BUFFER_MAX_NUM];
					memset(dummy, 0, sizeof(TransformType) * POINT_LIGHT_BUFFER_MAX_NUM);
					_transformBuffer.Initialize(dx, maxLightCount, tfFormat, dummy);

					_commonBuffer.Initialize(dx, maxLightCount, dummy);

					_mustUpdateTransformSRBuffer = true;
					_mustUpdateCommonSRBuffer = true;
				}

				void AddLight(const LightType& light)
				{
					_transformBuffer.AddData(light.GetLightId().Literal(), LightType::TransformType());
					_commonBuffer.AddData(light.GetBase(), -1, -1);

					_mustUpdateTransformSRBuffer = true;
					_mustUpdateCommonSRBuffer = true;
				}

				void UpdateTransformBuffer(const std::vector<LightType*>& dirtyTFLights,
											const Core::TransformPool& tfPool)
				{
					for (const auto& light : dirtyTFLights)
					{
						Core::ObjectId objId = light->GetObjectId();
						const auto& tf = tfPool.Find(objId.Literal());

						uint lightId = light->GetBase().GetLightId().Literal();
						_transformBuffer.SetData(lightId, light->MakeTransform(*tf));
					}

					_mustUpdateTransformSRBuffer |= (dirtyTFLights.empty() != false);
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

					_mustUpdateTransformSRBuffer = true;
					_mustUpdateCommonSRBuffer = true;
				}
				
				void DeleteAll()
				{
					_transformBuffer.DeleteAll();
					_commonBuffer.DeleteAll();

					_mustUpdateTransformSRBuffer = true;
					_mustUpdateCommonSRBuffer = true;
				}
				
			public:
				GET_ACCESSOR(TransformSRBuffer,				auto&, _transformBuffer.GetShaderResourceBuffer());
				GET_ACCESSOR(ColorSRBuffer,					auto&, _commonBuffer.GetColorSRBuffer());
				GET_ACCESSOR(OptionalParamIndexSRBuffer, 	auto&, _commonBuffer.GetOptionalParamIndexSRBuffer());
				GET_CONST_ACCESSOR(IndexBook,				const auto&, _transformBuffer.IndexBook());

				GET_CONST_ACCESSOR(Size, uint, _transformBuffer.GetSize());

			protected:
				TransformBuffer					_transformBuffer;
				CommonLightBuffer				_commonBuffer;

				bool							_mustUpdateCommonSRBuffer		= true;
				bool							_mustUpdateTransformSRBuffer	= true;
			};
		}
	}
}