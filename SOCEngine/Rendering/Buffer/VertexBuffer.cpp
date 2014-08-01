#include "VertexBuffer.h"

using namespace Rendering::Buffer;
using namespace Device;

VertexBuffer::VertexBuffer() : BaseBuffer()
{
}

VertexBuffer::~VertexBuffer()
{
}

bool VertexBuffer::Create( const void* sysMem, unsigned int byteWidth, bool isDynamic)
{	
	return BaseBuffer::Create(D3D11_BIND_VERTEX_BUFFER, 
		isDynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT, 
		sysMem, byteWidth);
}

void VertexBuffer::UpdateBuffer()
{
	ID3D11DeviceContext* context;
	context->IASetVertexBuffers(
}