#include "CPUReadBuffer.h"
#include "Director.h"
#include "ConstBuffer.h"

using namespace Rendering::Buffer;
using namespace Device;

CPUReadBuffer::CPUReadBuffer() : BaseBuffer()
{
}

CPUReadBuffer::~CPUReadBuffer()
{
}

bool CPUReadBuffer::Initialize(unsigned int stride, unsigned int num)
{
	D3D11_BUFFER_DESC desc;
	memset(&desc, 0, sizeof(D3D11_BUFFER_DESC));
	desc.ByteWidth				= stride * num;
	desc.Usage					= D3D11_USAGE_STAGING;
	desc.CPUAccessFlags			= D3D11_CPU_ACCESS_READ;

	ID3D11Device* device = Director::SharedInstance()->GetDirectX()->GetDevice();
	HRESULT hr = device->CreateBuffer(&desc, nullptr, &_buffer);

	ASSERT_COND_MSG(SUCCEEDED(hr), "Error!. Not create constant buffer");

	return true;
}

void CPUReadBuffer::Read(ID3D11DeviceContext* context, ID3D11Resource* target, const std::function<void(const void* dataRecive)>& dataReceiveFunc)
{
	context->CopyResource(_buffer, target);

	D3D11_MAPPED_SUBRESOURCE mappedData;
	context->Map(_buffer, 0, D3D11_MAP_READ, 0, &mappedData);
	dataReceiveFunc(mappedData.pData);
	context->Unmap(_buffer, 0);
}