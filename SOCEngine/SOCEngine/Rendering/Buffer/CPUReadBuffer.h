#pragma once

#include "BaseBuffer.h"
#include <functional>
#include "UnorderedAccessView.h"
#include "Common.h"
#include <memory>

namespace Rendering
{
	namespace Buffer
	{
		class CPUReadBuffer final
		{
		public:
			CPUReadBuffer() = default;

			void Initialize(Device::DirectX& dx, uint stride, uint num, DXGI_FORMAT format);
			void Read(Device::DirectX& dx, const std::function<void(const void* dataRecive)>& dataReceiveFunc);

			GET_ACCESSOR_REF(Buffer,	_baseBuffer);
			GET_ACCESSOR_REF(UAV,		_uav);

		private:
			DXSharedResource<ID3D11Buffer>	_readBuffer;
			BaseBuffer						_baseBuffer;
			View::UnorderedAccessView		_uav;
		};

	}
}
