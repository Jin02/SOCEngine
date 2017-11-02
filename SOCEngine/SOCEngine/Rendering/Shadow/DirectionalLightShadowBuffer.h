#pragma once

#include "ShadowBufferForm.hpp"
#include "DirectionalLightShadow.h"

namespace Rendering
{
	namespace Shadow
	{
		namespace Buffer
		{
			class DirectionalLightShadowBuffer final : public ShadowBufferForm<DirectionalLightShadow>
			{
			public:
				DirectionalLightShadowBuffer() = default;
				~DirectionalLightShadowBuffer() { DeleteAll(); }

				void Initialize(Device::DirectX& dx);
				void PushShadow(DirectionalLightShadow& shadow);
				void UpdateBuffer(	const std::vector<DirectionalLightShadow*>& dirtyShadows,
									const Light::LightPool<Light::DirectionalLight>& lightPool,
									const Core::TransformPool& tfPool,
									const ShadowDatasIndexer& indexer,
									const Intersection::BoundBox& sceneBoundBox);
				void UpdateSRBuffer(Device::DirectX& dx, bool forcedUpdate);

				void Delete(uint index);
				void DeleteAll();

				GET_ALL_ACCESSOR(InvProjParamSRBuffer, auto&, _dlParamBuffer.GetShaderResourceBuffer());

			private:
				using Parent = Buffer::ShadowBufferForm<Shadow::DirectionalLightShadow>;
				using Parent::Initialize;
				using Parent::UpdateBuffer;
				using Parent::UpdateSRBuffer;
				using Parent::Delete;
				using Parent::DeleteAll;

			private:
				Rendering::Buffer::GPUUploadBuffer<DirectionalLightShadow::Param> _dlParamBuffer;
				bool _mustUpdateDLParamSRBuffer = true;
			};
		}
	}
}
