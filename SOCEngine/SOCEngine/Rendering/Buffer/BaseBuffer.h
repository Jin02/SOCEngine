#pragma once

#include "DirectX.h"
#include "Common.h"

namespace Rendering
{
	namespace Buffer
	{
		class BaseBuffer
		{
		public:
			using Key = uint;

		public:
			BaseBuffer() = default;
			BaseBuffer(const DXSharedResource<ID3D11Buffer>& buffer) : _buffer(buffer) {}

			SET_ACCESSOR(Buffer,		const DXSharedResource<ID3D11Buffer>&,		_buffer);
			GET_CONST_ACCESSOR(Buffer,	DXSharedResource<ID3D11Buffer>,				_buffer);

			GET_ACCESSOR(Raw,			ID3D11Buffer* const,						_buffer.GetRaw());

			void UpdateSubResource(Device::DirectX& dx, const void* data);
			void UpdateResourceUsingMapUnMap(Device::DirectX& dx, const void* data, uint size);
			void UpdateResourceUsingMapUnMap(Device::DirectX& dx, const void* data, uint startOffset, uint size, D3D11_MAP mapType);

		private:
			DXSharedResource<ID3D11Buffer> _buffer;
		};
	}
}
