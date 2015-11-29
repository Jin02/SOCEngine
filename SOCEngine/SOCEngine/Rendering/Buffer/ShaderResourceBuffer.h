#pragma once

#include "BaseBuffer.h"

namespace Rendering
{
	namespace Buffer
	{
		class ShaderResourceBuffer : public Rendering::Buffer::BaseBuffer
		{
		private:
			ID3D11ShaderResourceView*	_srv;

		public:
			ShaderResourceBuffer();
			virtual ~ShaderResourceBuffer();

		public:
			void Initialize(uint stride, uint num, DXGI_FORMAT format, const void* sysMem, bool useMapWriteNoOverWrite, uint optionalBindFlag, D3D11_USAGE usage);

		public:
			GET_ACCESSOR(ShaderResourceView, ID3D11ShaderResourceView* const*, &_srv);
		};
	}
}
