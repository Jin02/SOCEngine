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
			template <class ShadowType>
			class ShadowBufferForm
			{
			public:
				using ViewProjMatType = typename ShadowType::ViewProjMatType;
				using TransformBuffer = Rendering::Buffer::GPUUploadBuffer<ViewProjMatType>;
				using ParamBuffer = Rendering::Buffer::GPUUploadBuffer<BaseShadow::Param>;

			public:
				ShadowBufferForm() = default;

				void Initialize(Device::DirectX& dx, uint maxShadowCount)
				{
					ViewProjMatType dummy[POINT_LIGHT_BUFFER_MAX_NUM];
					memset(dummy, 0, sizeof(dummy));

					_tfBuffer.Initialize(dx, maxShadowCount, DXGI_FORMAT_UNKNOWN, dummy);
					_paramBuffer.Initialize(dx, maxShadowCount, DXGI_FORMAT_R32G32B32A32_UINT, dummy);
				}
				void AddShadow(ShadowType& shadow)
				{
					const auto& base = shadow.GetBase();
					uint shadowId = base.GetShadowId().Literal();

					_tfBuffer.AddData(shadowId, ShadowType::ViewProjMatType());
					_paramBuffer.AddData(shadowId, base.GetParam());

					_mustUpdateTransformSRBuffer =
						_mustUpdateParamSRBuffer = true;
				}
				void UpdateBuffer(	const std::vector<ShadowType*>& dirtyShadows,
									const Light::LightPool<typename ShadowType::LightType>& lightPool,
									const Core::TransformPool& tfPool	)
				{
					for (auto& shadow : dirtyShadows)
					{
						const auto& base = shadow->GetBase();
						uint id = base.GetShadowId().Literal();

						_paramBuffer.SetData(id, base.GetParam());
						_tfBuffer.SetData(id, shadow->MakeVPMatParam(lightPool, tfPool));
					}

					_mustUpdateParamSRBuffer |= (dirtyShadows.empty() != false);
					_mustUpdateTransformSRBuffer |= _mustUpdateParamSRBuffer;
				}
				void UpdateSRBuffer(Device::DirectX& dx)
				{
					if (_mustUpdateTransformSRBuffer)	_tfBuffer.UpdateSRBuffer(dx);
					if (_mustUpdateParamSRBuffer)		_paramBuffer.UpdateSRBuffer(dx);

					_mustUpdateTransformSRBuffer =
						_mustUpdateParamSRBuffer = false;
				}
				void Delete(const ShadowType& shadow)
				{
					uint shadowId = shadow.GetBase().GetShadowId().Literal();

					_tfBuffer.Delete(shadowId);
					_paramBuffer.Delete(shadowId);

					_mustUpdateTransformSRBuffer =
						_mustUpdateParamSRBuffer = true;
				}
				void DeleteAll()
				{
					_tfBuffer.DeleteAll();
					_paramBuffer.DeleteAll();

					_mustUpdateTransformSRBuffer =
						_mustUpdateParamSRBuffer = true;
				}

				GET_ACCESSOR(ViewProjMatSRBuffer, auto&, _tfBuffer.GetShaderResourceBuffer());
				GET_ACCESSOR(ParamSRBuffer, auto&, _paramBuffer.GetShaderResourceBuffer());

			protected:
				TransformBuffer	_tfBuffer;
				ParamBuffer		_paramBuffer;
				bool			_mustUpdateTransformSRBuffer = true;
				bool			_mustUpdateParamSRBuffer = true;
			};
		}
	}
}