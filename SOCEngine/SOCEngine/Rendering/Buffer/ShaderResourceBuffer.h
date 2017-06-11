#pragma once

#include "BaseBuffer.h"
#include "ShaderResourceView.h"

namespace Rendering
{
	namespace Buffer
	{
		class ShaderResourceBuffer
		{
		public:
			ShaderResourceBuffer() = default;
			ShaderResourceBuffer(const BaseBuffer&, const View::ShaderResourceView&);

			void Initialize(Device::DirectX& dx, uint stride, uint num, DXGI_FORMAT format, const void* sysMem, bool useMapWriteNoOverWrite, uint optionalBindFlag, D3D11_USAGE usage);

			inline void UpdateResourceUsingMapUnMap(Device::DirectX& dx, const void* data, uint size)
			{
				_baseBuffer.UpdateResourceUsingMapUnMap(dx, data, size);
			}
			inline void UpdateResourceUsingMapUnMap(Device::DirectX& dx, const void* data, uint startOffset, uint size, D3D11_MAP mapType)
			{
				_baseBuffer.UpdateResourceUsingMapUnMap(dx, data, startOffset, size, mapType);
			}

			GET_ACCESSOR(ShaderResourceView, View::ShaderResourceView&, _srv);
			GET_CONST_ACCESSOR(BaseBuffer, const BaseBuffer&, _baseBuffer);

		private:
			BaseBuffer					_baseBuffer;
			View::ShaderResourceView	_srv;
		};
	}

}
