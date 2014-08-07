#include "BaseBuffer.h"
#include "Director.h"

using namespace Rendering::Buffer;
using namespace Device;

BaseBuffer::BaseBuffer() : _buffer(nullptr)
{
}

BaseBuffer::~BaseBuffer()
{
}

bool BaseBuffer::Create(unsigned int bindFlags, D3D11_USAGE usage, const void* sysMem, unsigned int byteWidth)
{
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = usage;
	bufferDesc.ByteWidth = byteWidth;
	bufferDesc.BindFlags = bindFlags;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA* initData = nullptr;
	if(bindFlags == D3D11_BIND_INDEX_BUFFER || bindFlags == D3D11_BIND_VERTEX_BUFFER)
	{
		D3D11_SUBRESOURCE_DATA data;
		initData = &data;

		memset(&data, 0, sizeof(D3D11_SUBRESOURCE_DATA));
		data.pSysMem = sysMem;
	}

	ID3D11Device* device = Director::GetInstance()->GetDirectX()->GetDevice();
	HRESULT hr = device->CreateBuffer(&bufferDesc, initData, &_buffer);
	if( FAILED( hr ) )
		return false;

	return true;
}