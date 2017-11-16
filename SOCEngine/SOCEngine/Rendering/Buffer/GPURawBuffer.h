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
			
			GET_ACCESSOR_REF(UnorderedAccessView,	_uav);
			GET_ACCESSOR_REF(ShaderResourceView,	_srv);
			GET_ACCESSOR_REF(BaseBuffer,			_buffer);

		private:
			View::UnorderedAccessView		_uav;
			View::ShaderResourceView		_srv;
			Buffer::BaseBuffer				_buffer;
		};
	}
}
