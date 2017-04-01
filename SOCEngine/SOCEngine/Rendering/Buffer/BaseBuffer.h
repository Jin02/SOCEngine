#pragma once

#include "DirectX.h"
#include "Common.h"
#include <memory>

namespace Rendering
{
	namespace Buffer
	{
		class BaseBuffer
		{
		public:
			BaseBuffer();
			BaseBuffer(const DXResource<ID3D11Buffer>& buffer);

			SET_ACCESSOR(Buffer, const DXResource<ID3D11Buffer>&, _base);
			GET_CONST_ACCESSOR(Buffer, DXResource<ID3D11Buffer>, _base);

			void UpdateSubResource(Device::DirectX& dx, const void* data);
			void UpdateResourceUsingMapUnMap(Device::DirectX& dx, const void* data, uint size);
			void UpdateResourceUsingMapUnMap(Device::DirectX& dx, const void* data, uint startOffset, uint size, D3D11_MAP mapType);

		private:
			DXResource<ID3D11Buffer>	_base;
		};
	}
}