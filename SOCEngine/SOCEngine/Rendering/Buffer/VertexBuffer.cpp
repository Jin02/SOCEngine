#include "VertexBuffer.h"
#include "Director.h"

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

	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = isDynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_IMMUTABLE;
	bufferDesc.ByteWidth = bufferSize * count;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data;
	memset(&data, 0, sizeof(D3D11_SUBRESOURCE_DATA));
	data.pSysMem = sysMem;

	ID3D11Device* device = Director::GetInstance()->GetDirectX()->GetDevice();
	HRESULT hr = device->CreateBuffer(&bufferDesc, &data, &_buffer);

	ASSERT_COND_MSG(SUCCEEDED(hr), "Error!. does not create vb");

	return true;
}

void VertexBuffer::IASetBuffer(ID3D11DeviceContext* context)
{
	unsigned int offset = 0;
	context->IASetVertexBuffers(0, 1, &_buffer, &_stride, &offset); 
}