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
			struct RequiredIndexer
			{
				const Core::ObjectId::IndexHashMap& shadowIndexer;
				const Core::ObjectId::IndexHashMap& lightShaftIndexer;
				RequiredIndexer(
					const Core::ObjectId::IndexHashMap& _shadowIndexer,
					const Core::ObjectId::IndexHashMap& _lightShaftIndexer)
					: shadowIndexer(_shadowIndexer), lightShaftIndexer(_lightShaftIndexer)
				{

				}
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

				void PushLight(const LightType& light)
				{
					_transformBuffer.PushData(LightType::TransformType());
					_commonBuffer.PushData(light.GetBase(), -1, -1);

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
				void UpdateTransformBuffer(	const std::vector<LightType*>& dirtyTFLights,
											const Core::TransformPool& tfPool,
											const Core::ObjectId::IndexHashMap& indexer)
				{
					for (const auto& light : dirtyTFLights)
					{
						Core::ObjectId objId = light->GetObjectId();
						const auto& tf = tfPool.Find(objId.Literal());
						
						uint index = indexer.Find(objId.Literal());
						_transformBuffer[index] = light->MakeTransform(*tf);
					}

					_mustUpdateTransformSRBuffer |= (dirtyTFLights.empty() != false);
				}

				void UpdateLightCommonBuffer(const std::vector<LightType*>& dirtyParamLights, RequiredIndexer indexers, const Core::ObjectId::IndexHashMap& indexer)
				{
					for (auto& light : dirtyParamLights)
					{
						Core::ObjectId objId = light->GetObjectId();
						uint literalId = objId.Literal();

						ushort shadowIdx = indexers.shadowIndexer.Find(literalId);
						uint lightShaftIdx = indexers.lightShaftIndexer.Find(literalId);

						uint index = indexer.Find(literalId);
						_commonBuffer.SetData(index, light->GetBase(), shadowIdx, lightShaftIdx);
					}

					_mustUpdateCommonSRBuffer |= (dirtyParamLights.empty() != false);
				}

				void UpdateSRBuffer(Device::DirectX& dx)
				{
					if(_mustUpdateTransformSRBuffer)		_transformBuffer.UpdateSRBuffer(dx);
					if(_mustUpdateCommonSRBuffer)			_commonBuffer.UpdateSRBuffer(dx);

					_mustUpdateCommonSRBuffer =
					_mustUpdateTransformSRBuffer = false;
				}
				void Delete(uint index)
				{
					_transformBuffer.Delete(index);
					_commonBuffer.Delete(index);

					_mustUpdateTransformSRBuffer = true;
					_mustUpdateCommonSRBuffer = true;
				}
				
				
			public:
				GET_ACCESSOR(TransformSRBuffer,				auto&, _transformBuffer.GetShaderResourceBuffer());
				GET_ACCESSOR(ColorSRBuffer,					auto&, _commonBuffer.GetColorSRBuffer());
				GET_ACCESSOR(OptionalParamIndexSRBuffer, 	auto&, _commonBuffer.GetOptionalParamIndexSRBuffer());

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