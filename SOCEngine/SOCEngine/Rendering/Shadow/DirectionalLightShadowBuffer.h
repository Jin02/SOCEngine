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

				void UpdateBuffer(	const std::vector<DirectionalLightShadow*>& dirtyShadows,
									const Light::LightPool<Light::DirectionalLight>& lightPool,
									const Core::TransformPool& tfPool,
									const ShadowDatasIndexer& indexer,
									const Intersection::BoundBox& sceneBoundBox);

			private:
				using Parent = Buffer::ShadowBufferForm<Shadow::DirectionalLightShadow>;
				using Parent::UpdateBuffer;
			};
		}
	}
}
