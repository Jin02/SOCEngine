#include "OutputBuffer.h"
#include "Director.h"

using namespace Device;
using namespace GPGPU::DirectCompute;

OutputBuffer::OutputBuffer() : BaseBuffer()
{

}

OutputBuffer::~OutputBuffer()
{

}

bool OutputBuffer::Create(unsigned int size, unsigned int stride)
{
	D3D11_BUFFER_DESC desc;
    desc.ByteWidth = size;
	desc.StructureByteStride = stride;
    desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	desc.CPUAccessFlags = 0;

	ID3D11Device* device = Director::GetInstance()->GetDirectX()->GetDevice();
	HRESULT hr = device->CreateBuffer(&desc, nullptr, &_buffer);

	if( FAILED( hr ) )
	{
		ASSERT("Error!. Not create constant buffer");
		return false;
	}

	return true;
}