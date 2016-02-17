#include "CPUReadBuffer.h"
#include "Director.h"
#include "ConstBuffer.h"

using namespace Rendering::View;
using namespace Rendering::Buffer;
using namespace Device;

CPUReadBuffer::CPUReadBuffer() : BaseBuffer(), _uav(nullptr), _readBuffer(nullptr)
{
}

CPUReadBuffer::~CPUReadBuffer()
{
	SAFE_DELETE(_uav);
	SAFE_RELEASE(_readBuffer);
}

bool CPUReadBuffer::Initialize(uint stride, uint num, DXGI_FORMAT format)
{
	D3D11_BUFFER_DESC desc;
	memset(&desc, 0, sizeof(D3D11_BUFFER_DESC));
	desc.ByteWidth				= stride * num;
	desc.Usage					= D3D11_USAGE_DEFAULT;
	desc.BindFlags				= D3D11_BIND_UNORDERED_ACCESS;

	if(format == DXGI_FORMAT_UNKNOWN)
	{
		desc.StructureByteStride	= stride;
		desc.MiscFlags				= D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	}

	ID3D11Device* device = Director::SharedInstance()->GetDirectX()->GetDevice();
	HRESULT hr = device->CreateBuffer(&desc, nullptr, &_buffer);

	ASSERT_COND_MSG(SUCCEEDED(hr), "Error!. Not create buffer");

	desc.Usage					= D3D11_USAGE_STAGING;
	desc.BindFlags				= 0;
	desc.CPUAccessFlags			= D3D11_CPU_ACCESS_READ;
	hr = device->CreateBuffer(&desc, nullptr, &_readBuffer);

	ASSERT_COND_MSG(SUCCEEDED(hr), "Error!. Not create staging buffer");

	_uav = new UnorderedAccessView;
	_uav->Initialize(format, num, _buffer, D3D11_UAV_DIMENSION_BUFFER);

	return true;
}

void CPUReadBuffer::Read(ID3D11DeviceContext* context, const std::function<void(const void* dataRecive)>& dataReceiveFunc)
{
	context->CopyResource(_readBuffer, _buffer);

	D3D11_MAPPED_SUBRESOURCE mappedData;
	context->Map(_readBuffer, 0, D3D11_MAP_READ, 0, &mappedData);
	dataReceiveFunc(mappedData.pData);
	context->Unmap(_readBuffer, 0);
}