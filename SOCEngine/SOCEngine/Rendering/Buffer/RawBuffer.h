#pragma once

#include "ShaderResourceBuffer.h"
#include "UnorderedAccessView.h"

namespace Rendering
{
	namespace Buffer
	{
		class RawBuffer final
		{
		public:
			enum class Flag
			{
				ONLY_UAV,
				ONLY_SRV,
				ALL_VIEW
			};

			RawBuffer() = default;

			void Initialize(Device::DirectX& dx, uint stride, uint elemNum, Flag flag);
			
			GET_CONST_ACCESSOR(UnorderedAccessView, const View::UnorderedAccessView&, _uav);
			GET_CONST_ACCESSOR(ShaderResourceView, const View::ShaderResourceView&, _srv);
			GET_CONST_ACCESSOR(BaseBuffer, const BaseBuffer&, _buffer);

		private:
			View::UnorderedAccessView		_uav;
			View::ShaderResourceView		_srv;
			Buffer::BaseBuffer				_buffer;
		};
	}
}
