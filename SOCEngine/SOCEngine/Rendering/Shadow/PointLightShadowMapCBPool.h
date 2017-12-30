#pragma once

#include <array>
#include <vector>

#include "ConstBuffer.h"
#include "Matrix.h"
#include "PointLightShadow.h"
#include "ShadowBufferForm.hpp"

namespace Rendering
{
	namespace Shadow
	{
		namespace Buffer
		{
			class PointLightShadowMapCBPool
			{
			public:
				using ConstBufferType = std::array<Rendering::Buffer::ExplicitConstBuffer<Math::Matrix>, 6>;

			public:
				void PushConstBufferToQueue();
				void InitializePreparedCB(Device::DirectX& dx);

				void Delete(uint index);
				void DeleteAll();
				
				void UpdateSubResource(Device::DirectX& dx,
					const std::vector<Shadow::PointLightShadow*>& dirtyShadows,
					const ShadowDatasIndexer& indexer);

				const auto& Get(uint index) const	{ return _constBuffers[index]; }
				auto& Get(uint index)				{ return _constBuffers[index]; }

			private:
				std::vector<ConstBufferType>	_preparedConstBuffers;
				std::vector<ConstBufferType>	_constBuffers;
			};
		}
	}
}
