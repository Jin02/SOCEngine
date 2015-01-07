#include "ConstBuffer_CPURead.h"
#include "Director.h"
#include "ConstBuffer.h"

using namespace Rendering::Buffer;
using namespace Device;

ConstBuffer_CPURead::ConstBuffer_CPURead() : BaseBuffer()
{
}

ConstBuffer_CPURead::~ConstBuffer_CPURead()
{
}

bool ConstBuffer_CPURead::Create(unsigned int stride, unsigned int num)
{
	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = stride * num;
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

void ConstBuffer_CPURead::Read(ID3D11DeviceContext* context, BaseBuffer* target, const std::function<void(const void* dataRecive)>& dataReceiveFunc)
{
	context->CopyResource(_buffer, target->GetBuffer());

	D3D11_MAPPED_SUBRESOURCE mappedData;
	context->Map(_buffer, 0, D3D11_MAP_READ, 0, &mappedData);
	dataReceiveFunc(mappedData.pData);
	context->Unmap(_buffer, 0);
}