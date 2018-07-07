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
			void Destroy();

			GET_CONST_ACCESSOR_REF(ShaderResourceView,	_srv);
			GET_CONST_ACCESSOR_REF(BaseBuffer,			_buffer);

			GET_ALL_ACCESSOR_PTR(UnorderedAccessView, View::UnorderedAccessView, _uav);

		private:
			View::UnorderedAccessView		_uav;
			View::ShaderResourceView		_srv;
			Buffer::BaseBuffer				_buffer;
		};
	}
}
