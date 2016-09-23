#pragma once

#include "BaseBuffer.h"
#include "ShaderResourceView.h"

namespace Rendering
{
	namespace Buffer
	{
		class ShaderResourceBuffer : public Rendering::Buffer::BaseBuffer
		{
		protected:
			View::ShaderResourceView*	_srv;

		public:
			ShaderResourceBuffer();
			virtual ~ShaderResourceBuffer();

		public:
			void Initialize(uint stride, uint num, DXGI_FORMAT format, const void* sysMem, bool useMapWriteNoOverWrite, uint optionalBindFlag, D3D11_USAGE usage);
			void Destroy();

		public:
			GET_ACCESSOR(ShaderResourceView, const View::ShaderResourceView*, _srv);
		};
	}
}
