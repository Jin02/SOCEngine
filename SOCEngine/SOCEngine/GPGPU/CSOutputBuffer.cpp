#include "CSOutputBuffer.h"
#include "Director.h"

using namespace Device;
using namespace GPGPU::DirectCompute;

CSOutputBuffer::CSOutputBuffer() : BaseBuffer(), _uav(nullptr)
{

}

CSOutputBuffer::~CSOutputBuffer()
{

}

bool CSOutputBuffer::Create(unsigned int stride, unsigned int num)
{
	D3D11_BUFFER_DESC desc;
    desc.ByteWidth = stride * num;
	desc.StructureByteStride = stride;
    desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	desc.CPUAccessFlags = 0;

	ID3D11Device* device = Director::GetInstance()->GetDirectX()->GetDevice();
	HRESULT hr = device->CreateBuffer(&desc, nullptr, &_buffer);

	if( FAILED( hr ) )
	{
		ASSERT("Error!. does not create constant buffer");
		return false;
	}

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.Flags = 0;
	uavDesc.Buffer.NumElements = num;

	hr = device->CreateUnorderedAccessView(_buffer, &uavDesc, &_uav);
	if( FAILED(hr) )
	{
		ASSERT("Error!, does not create unordered acess view");
		return false;
	}

	return true;
}