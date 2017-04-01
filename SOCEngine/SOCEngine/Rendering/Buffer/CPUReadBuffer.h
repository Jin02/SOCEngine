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
		class ConstBuffer;

		class CPUReadBuffer final
		{
		public:
			CPUReadBuffer();
			~CPUReadBuffer();

			DISALLOW_ASSIGN_COPY(CPUReadBuffer);

			bool Initialize(Device::DirectX& dx, uint stride, uint num, DXGI_FORMAT format);
			void Read(Device::DirectX& dx, const std::function<void(const void* dataRecive)>& dataReceiveFunc);

			GET_CONST_ACCESSOR(Buffer, const BaseBuffer&, _baseBuffer);
			GET_CONST_ACCESSOR(UAV, const View::UnorderedAccessView&, _uav);

		private:
			DXResource<ID3D11Buffer>				_readBuffer;
			BaseBuffer								_baseBuffer;
			View::UnorderedAccessView				_uav;
		};

	}
}