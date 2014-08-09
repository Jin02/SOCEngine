#include "IndexBuffer.h"

using namespace Rendering::Buffer;
using namespace Device;

IndexBuffer::IndexBuffer() : BaseBuffer()
{
}

IndexBuffer::~IndexBuffer()
{
}

bool IndexBuffer::Create(const ENGINE_INDEX_TYPE* sysMem, unsigned int byteWidth)
{
	return BaseBuffer::Create(D3D11_BIND_INDEX_BUFFER, D3D11_USAGE_DEFAULT, sysMem, byteWidth);
}

void IndexBuffer::UpdateBuffer(ID3D11DeviceContext* context)
{
	context->IASetIndexBuffer(_buffer, DXGI_FORMAT_R16_UINT, 0);
}