#include "ConstBuffer.h"
#include "Director.h"
#include <type_traits> 

using namespace Rendering::Buffer;
using namespace Device;

ConstBuffer::ConstBuffer() : BaseBuffer()
{
}

ConstBuffer::~ConstBuffer()
{
}

bool ConstBuffer::CreateUsageVSPS(unsigned int size)
{
	D3D11_BUFFER_DESC desc;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = size;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	ID3D11Device* device = Director::GetInstance()->GetDirectX()->GetDevice();
	HRESULT hr = device->CreateBuffer(&desc, nullptr, &_buffer);

	if( FAILED( hr ) )
	{
		ASSERT("Error!. Not create constant buffer");
		return false;
	}

	return true;
}

bool ConstBuffer::CreateUsageStaging(unsigned int size, unsigned int stride)
{
	D3D11_BUFFER_DESC desc;
    desc.ByteWidth = size;
	desc.StructureByteStride = stride;
    desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.Usage = D3D11_USAGE_STAGING;
	desc.BindFlags = 0;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

	ID3D11Device* device = Director::GetInstance()->GetDirectX()->GetDevice();
	HRESULT hr = device->CreateBuffer(&desc, nullptr, &_buffer);

	if( FAILED( hr ) )
	{
		ASSERT("Error!. Not create constant buffer");
		return false;
	}

	return true;
}

bool ConstBuffer::Create(unsigned int size, unsigned int stride, Usage usage, const void* sysMem)
{
	size = 16 * (size/16) + ((size%16) != 0) * 16;

	switch(usage)
	{
	case Usage::VertexShader:
	case Usage::PixelShader:
		return CreateUsageVSPS(size);
	case Usage::Staging:
		return CreateUsageStaging(size, stride);
	}

	ASSERT("Error!, undefined constbuffer type");
	return false;
}

void ConstBuffer::UpdateSubresource(ID3D11DeviceContext* context, const void* data)
{
	context->UpdateSubresource(_buffer, 0, nullptr, data, 0, 0);
}