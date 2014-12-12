#include "InputBuffer.h"
#include "Director.h"

using namespace Device;
using namespace GPGPU::DirectCompute;

InputBuffer::InputBuffer() : BaseBuffer()
{

}

InputBuffer::~InputBuffer()
{

}

bool InputBuffer::Create(unsigned int size, unsigned int stride, const void* sysMem)
{
	D3D11_BUFFER_DESC desc;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.ByteWidth = size;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;
	desc.StructureByteStride = stride;
    desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = sysMem;

	ID3D11Device* device = Director::GetInstance()->GetDirectX()->GetDevice();
	HRESULT hr = device->CreateBuffer(&desc, &data, &_buffer);

	if( FAILED( hr ) )
	{
		ASSERT("Error!. Not create constant buffer");
		return false;
	}

	return true;
}