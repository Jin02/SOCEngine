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
			void Initialize(uint stride, uint num, DXGI_FORMAT format, const void* sysMem, bool useMapWriteNoOverWrite, uint optionalBindFlag, D3D11_USAGE usage, D3D11_RESOURCE_MISC_FLAG miscFlag);

		public:
			GET_ACCESSOR(ShaderResourceView, const View::ShaderResourceView*, _srv);
		};
	}
}
