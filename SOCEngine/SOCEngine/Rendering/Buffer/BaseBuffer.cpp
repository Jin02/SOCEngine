#include "BaseBuffer.h"
#include <assert.h>
#include <memory>

using namespace Rendering::Buffer;
using namespace Device;

void BaseBuffer::UpdateSubResource(DirectX& dx, const void* data)
{
	dx.GetContext()->UpdateSubresource(_buffer.GetRaw(), 0, nullptr, data, 0, 0);
}

void BaseBuffer::UpdateResourceUsingMapUnMap(DirectX& dx, const void* data, uint size)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ASSERT_SUCCEEDED(dx.GetContext()->Map(_buffer.GetRaw(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));

	memcpy(mappedResource.pData, data, size);

	dx.GetContext()->Unmap(_buffer.GetRaw(), 0);
}

void BaseBuffer::UpdateResourceUsingMapUnMap(DirectX& dx, const void* data, uint startOffset, uint size, D3D11_MAP mapType)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ASSERT_SUCCEEDED(dx.GetContext()->Map(_buffer.GetRaw(), 0, mapType, 0, &mappedResource));

	void* mem = reinterpret_cast<char*>(mappedResource.pData) + startOffset;
	memcpy(mem, data, size);

	dx.GetContext()->Unmap(_buffer.GetRaw(), 0);
}
