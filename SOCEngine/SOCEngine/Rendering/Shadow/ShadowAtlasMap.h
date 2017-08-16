#pragma once

#include "DepthBuffer.h"

namespace Rendering
{
	namespace Shadow
	{
		template <class ShadowType>
		class ShadowAtlasMap : public Texture::DepthBuffer
		{
		public:
			using Parent = Texture::DepthBuffer;
			using Parent::Parent;

			void Initialize(Device::DirectX& dx, const Size<uint>& size,
				uint resolution, uint capacity)
			{
				Parent::Initialize(dx, size, true, 1);

				_resolution = resolution;
				_capacity = capacity;
			}

			GET_CONST_ACCESSOR(Resolution, uint, _resolution);
			GET_CONST_ACCESSOR(Capacity, uint, _capacity);

		private:
			using Parent::Initialize;

		private:
			uint					_resolution		= 0;
			uint					_capacity		= 0;
		};
	}
}