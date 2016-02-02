#pragma once

#include "BaseBuffer.h"
#include <functional>
#include "UnorderedAccessView.h"

namespace Rendering
{
	namespace Buffer
	{
		class ConstBuffer;

		class CPUReadBuffer : private BaseBuffer
		{
		private:
			ID3D11Buffer*				_readBuffer;
			View::UnorderedAccessView*	_uav;

		public:
			CPUReadBuffer();
			virtual ~CPUReadBuffer();

		public:
			bool Initialize(uint stride, uint num, DXGI_FORMAT format);
			void Read(ID3D11DeviceContext* context, const std::function<void(const void* dataRecive)>& dataReceiveFunc);

		private:
			void Update(ID3D11DeviceContext* context, const void* data){}

		public:
			GET_ACCESSOR(Buffer,	ID3D11Buffer*,							_buffer);
			GET_ACCESSOR(UAV,		const View::UnorderedAccessView*,		_uav);
		};

	}
}