#pragma once

#include "GlobalDefine.h"
#include "GPUUploadBuffer.hpp"
#include "BaseShadow.h"
#include "BaseLight.h"
#include "Transform.h"

namespace Rendering
{
	namespace Shadow
	{
		namespace Buffer
		{
			using ShadowDatasIndexer = Core::IndexHashMap<Core::ObjectID::LiteralType>;

			template <class ShadowType>
			class ShadowBufferForm
			{
			public:
				using ViewProjMatType = typename ShadowType::ViewProjMatType;
				using ViewProjMatBuffer = Rendering::Buffer::GPUUploadBuffer<ViewProjMatType>;
				using ParamBuffer = Rendering::Buffer::GPUUploadBuffer<BaseShadow::Param>;

			public:
				ShadowBufferForm() = default;

				void Initialize(Device::DirectX& dx, uint maxShadowCount)
				{
					ViewProjMatType dummy[POINT_LIGHT_BUFFER_MAX_NUM];
					memset(dummy, 0, sizeof(dummy));

					_transformBuffer.Initialize(dx, maxShadowCount, DXGI_FORMAT_UNKNOWN, dummy);
					_paramBuffer.Initialize(dx, maxShadowCount, DXGI_FORMAT_R32G32B32A32_UINT, dummy);
				}
				void PushShadow(const ShadowType& shadow)
				{
					const auto& base = shadow.GetBase();

					_transformBuffer.PushData(ShadowType::ViewProjMatType());
					_paramBuffer.PushData(base.GetParam());

					_mustUpdateTransformSRBuffer =
						_mustUpdateParamSRBuffer = true;
				}
				void UpdateBuffer(	const std::vector<ShadowType*>& dirtyShadows,
									const Light::LightPool<typename ShadowType::LightType>& lightPool,
									const Core::TransformPool& tfPool,
									const ShadowDatasIndexer& indexer)
				{
					for (auto& shadow : dirtyShadows)
					{
						const auto& base = shadow->GetBase();
						Core::ObjectID objID = base.GetObjectID();

						uint index = indexer.Find(objID.Literal());
						_paramBuffer[index] = base.GetParam();
						_transformBuffer[index] = shadow->MakeVPMatParam(lightPool, tfPool);
					}

					_mustUpdateParamSRBuffer |= (dirtyShadows.empty() != false);
					_mustUpdateTransformSRBuffer |= _mustUpdateParamSRBuffer;
				}
				void UpdateSRBuffer(Device::DirectX& dx)
				{
					if (_mustUpdateTransformSRBuffer)	_transformBuffer.UpdateSRBuffer(dx);
					if (_mustUpdateParamSRBuffer)		_paramBuffer.UpdateSRBuffer(dx);

					_mustUpdateTransformSRBuffer =
						_mustUpdateParamSRBuffer = false;
				}
				void Delete(uint index)
				{
					_transformBuffer.Delete(index);
					_paramBuffer.Delete(index);

					_mustUpdateTransformSRBuffer =
						_mustUpdateParamSRBuffer = true;
				}
				void DeleteAll()
				{
					_transformBuffer.DeleteAll();
					_paramBuffer.DeleteAll();

					_mustUpdateTransformSRBuffer =
						_mustUpdateParamSRBuffer = true;
				}

				GET_ACCESSOR(ViewProjMatSRBuffer, auto&, _transformBuffer.GetShaderResourceBuffer());
				GET_ACCESSOR(ParamSRBuffer, auto&, _paramBuffer.GetShaderResourceBuffer());

			protected:
				ViewProjMatBuffer	_transformBuffer;
				ParamBuffer			_paramBuffer;
				bool				_mustUpdateTransformSRBuffer = true;
				bool				_mustUpdateParamSRBuffer = true;
			};
		}
	}
}