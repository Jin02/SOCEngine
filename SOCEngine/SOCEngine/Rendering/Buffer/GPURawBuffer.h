#pragma once

#include "ShaderResourceBuffer.h"
#include "UnorderedAccessView.h"

namespace Rendering
{
	namespace Buffer
	{
		class GPURawBuffer final
		{
		public:
			enum class Flag
			{
				ONLY_UAV,
				ONLY_SRV,
				ALL_VIEW
			};

		public:
			void Initialize(Device::DirectX& dx, uint stride, uint elemNum, Flag flag);
			
			GET_ALL_ACCESSOR(UnorderedAccessView,	auto&, _uav);
			GET_ALL_ACCESSOR(ShaderResourceView,	auto&, _srv);
			GET_ALL_ACCESSOR(BaseBuffer,			auto&, _buffer);

		private:
			View::UnorderedAccessView		_uav;
			View::ShaderResourceView		_srv;
			Buffer::BaseBuffer				_buffer;
		};
	}
}
