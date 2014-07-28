#include "IndexBuffer.h"

using namespace Rendering::Buffer;
using namespace Device;

IndexBuffer::IndexBuffer() : BaseBuffer()
{
}

IndexBuffer::~IndexBuffer()
{
}

bool IndexBuffer::Create(const WORD* sysMem, unsigned int byteWidth)
{
	return BaseBuffer::Create(D3D11_BIND_INDEX_BUFFER, D3D11_USAGE_DEFAULT, sysMem, byteWidth);
}