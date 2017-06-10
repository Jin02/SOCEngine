#pragma once

#include "LightingBufferForm.hpp"
#include "SpotLight.h"
#include <assert.h>

namespace Rendering
{
	namespace Light
	{
		namespace LightingBuffer
		{
			class SpotLightingBuffer final : public LightingBufferForm<SpotLight>
			{
			public:
				SpotLightingBuffer() = default;
				void Initialize(Device::DirectX& dx)
				{
					Parent::Initialize(dx, 2048, DXGI_FORMAT_R32G32B32A32_FLOAT);

					SpotLight::Param dummy[2048];
					_paramSRBuffer.Initialize(dx, 2048, DXGI_FORMAT_R16G16B16A16_FLOAT, dummy);
				}

				void RegistLight(const SpotLight& light, const Core::Transform& transform,
					const Core::IndexHashMap<LightId::LiteralType>& shadowIndexBook,
					const Core::IndexHashMap<LightId::LiteralType>& lightShaftIndexBook)
				{
					assert(light.GetObjectId() == transform.GetObjectId());

					_paramSRBuffer.RegistData(light.GetLightId().Literal(), light.MakeParam(transform));
					Parent::RegistLight(light, shadowIndexBook, lightShaftIndexBook);
				}

				void UpdateParamBuffer(const std::vector<SpotLight*>& dirtyLights, Core::TransformPool& transformPool)
				{
					for (auto& light : dirtyLights)
					{
						uint lightId = light->GetLightId().Literal();

						const auto& tf = transformPool.Find( light->GetObjectId().Literal() );
						assert(tf);

						_paramSRBuffer.SetData(lightId, light->MakeParam(*tf));
					}

					_mustUpdateParamSRBuffer |= (dirtyLights.empty() != false);
				}

				void UpdateSRBuffer(Device::DirectX& dx)
				{
					if(_mustUpdateParamSRBuffer)
						_paramSRBuffer.UpdateSRBuffer(dx);

					_mustUpdateParamSRBuffer = false;

					Parent::UpdateSRBuffer(dx);
				}

				void Delete(const SpotLight& light)
				{
					_paramSRBuffer.Delete(light.GetObjectId().Literal());
					Parent::Delete(light);
				}

				void DeleteAll()
				{
					_paramSRBuffer.DeleteAll();
					Parent::DeleteAll();
				}

			private:
				using Parent = LightingBufferForm<SpotLight>;
				using Parent::Initialize;
				using Parent::RegistLight;
				using Parent::UpdateSRBuffer;
				using Parent::Delete;
				using Parent::DeleteAll;

			private:		
				Buffer::GPUUploadBuffer<SpotLight::Param> _paramSRBuffer;
				bool _mustUpdateParamSRBuffer = true;
			};
		}
	}
}