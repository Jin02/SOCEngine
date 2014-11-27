#include "ConstBuffer.h"

using namespace Rendering::Buffer;
using namespace Device;

ConstBuffer::ConstBuffer() : BaseBuffer()
{
}

ConstBuffer::~ConstBuffer()
{
}

bool ConstBuffer::Create(unsigned int size)
{
	size = 16 * (size/16) + ((size%16) != 0) * 16;
	return BaseBuffer::Create(D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DEFAULT, nullptr, size);
}

void ConstBuffer::UpdateSubresource(ID3D11DeviceContext* context, const void* data)
{
	context->UpdateSubresource(_buffer, 0, nullptr, data, 0, 0);
}