#include "CPUReadBuffer.h"
#include <assert.h>

using namespace Rendering::View;
using namespace Rendering::Buffer;
using namespace Device;

CPUReadBuffer::CPUReadBuffer()
	: _readBuffer(nullptr), _baseBuffer(), _uav()
{
}

CPUReadBuffer::~CPUReadBuffer()
{
}

bool CPUReadBuffer::Initialize(Device::DirectX& dx, uint stride, uint num, DXGI_FORMAT format)
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

	_baseBuffer.SetBuffer(dx.CreateBuffer(desc, nullptr));

	desc.Usage					= D3D11_USAGE_STAGING;
	desc.BindFlags				= 0;
	desc.CPUAccessFlags			= D3D11_CPU_ACCESS_READ;

	ID3D11Buffer* buffer = nullptr;
	assert(SUCCEEDED(dx.GetDevice()->CreateBuffer(&desc, nullptr, &buffer)));
	_readBuffer = DXResource<ID3D11Buffer>(buffer);

	_uav.Initialize(dx, format, num, _baseBuffer.GetBuffer(), D3D11_UAV_DIMENSION_BUFFER);

	return true;
}

void CPUReadBuffer::Read(Device::DirectX& dx, const std::function<void(const void* dataRecive)>& dataReceiveFunc)
{
	auto context = dx.GetContext();
	context->CopyResource(_readBuffer.GetRaw(), _baseBuffer.GetBuffer().GetRaw());

	D3D11_MAPPED_SUBRESOURCE mappedData;
	context->Map(_readBuffer.GetRaw(), 0, D3D11_MAP_READ, 0, &mappedData);
	dataReceiveFunc(mappedData.pData);
	context->Unmap(_readBuffer.GetRaw(), 0);
}