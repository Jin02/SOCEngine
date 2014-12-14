#include "IndexBuffer.h"
#include "Director.h"

using namespace Rendering::Buffer;
using namespace Device;

IndexBuffer::IndexBuffer() : BaseBuffer()
{
}

IndexBuffer::~IndexBuffer()
{
}

bool IndexBuffer::Create(const ENGINE_INDEX_TYPE* sysMem, unsigned int byteWidth, bool isDynamic)
{
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = isDynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_IMMUTABLE;
	bufferDesc.ByteWidth = byteWidth;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data;
	memset(&data, 0, sizeof(D3D11_SUBRESOURCE_DATA));
	data.pSysMem = sysMem;

	ID3D11Device* device = Director::GetInstance()->GetDirectX()->GetDevice();
	HRESULT hr = device->CreateBuffer(&bufferDesc, &data, &_buffer);

	if( FAILED( hr ) )
	{
		ASSERT("Error!. does not create ib");
		return false;
	}

	return true;
}

void IndexBuffer::IASetBuffer(ID3D11DeviceContext* context)
{
	context->IASetIndexBuffer(_buffer, DXGI_FORMAT_R32_UINT, 0);
}