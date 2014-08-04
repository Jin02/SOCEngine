#include "VertexBuffer.h"

using namespace Rendering::Buffer;
using namespace Device;

VertexBuffer::VertexBuffer() : BaseBuffer()
{
}

VertexBuffer::~VertexBuffer()
{
}

bool VertexBuffer::Create( const void* sysMem, unsigned int bufferSize, unsigned int count, bool isDynamic)
{	
	_stride = bufferSize;

	return BaseBuffer::Create(D3D11_BIND_VERTEX_BUFFER, 
		isDynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT, 
		sysMem, bufferSize * count);
}

void VertexBuffer::UpdateBuffer()
{
	ID3D11DeviceContext* context;
	unsigned int offset = 0;
	context->IASetVertexBuffers(0, 1, &_buffer, &_stride, &offset); 
}