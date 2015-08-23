#include "BaseBuffer.h"
#include "Director.h"

using namespace Rendering::Buffer;
using namespace Device;

BaseBuffer::BaseBuffer() : _buffer(nullptr)
{
}

BaseBuffer::~BaseBuffer()
{
	SAFE_RELEASE(_buffer);
}

void BaseBuffer::UpdateSubResource(ID3D11DeviceContext* context, const void* data)
{
	context->UpdateSubresource(_buffer, 0, nullptr, data, 0, 0);
}

void BaseBuffer::UpdateResourceUsingMapUnMap(ID3D11DeviceContext* context, const void* data, uint size)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = context->Map(_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	memcpy(mappedResource.pData, data, size);

	context->Unmap(_buffer, 0);
}

void BaseBuffer::UpdateResourceUsingMapUnMap(ID3D11DeviceContext* context, const void* data, uint startOffset, uint size, D3D11_MAP mapType)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = context->Map(_buffer, 0, mapType, 0, &mappedResource);

	void* mem = ((char*)mappedResource.pData + startOffset);
	memcpy(mem, data, size);

	context->Unmap(_buffer, 0);
}