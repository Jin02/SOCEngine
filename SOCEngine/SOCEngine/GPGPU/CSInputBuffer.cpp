#include "CSInputBuffer.h"
#include "Director.h"

using namespace Device;
using namespace GPGPU::DirectCompute;

CSInputBuffer::CSInputBuffer() : BaseBuffer(), _srv(nullptr)
{

}

CSInputBuffer::~CSInputBuffer()
{
	SAFE_RELEASE(_srv);
}

bool CSInputBuffer::Initialize(unsigned int stride, unsigned int num, const void* sysMem)
{
	D3D11_BUFFER_DESC desc;
	memset(&desc, 0, sizeof(D3D11_BUFFER_DESC));

	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.ByteWidth = stride * num;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.StructureByteStride = stride;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = sysMem;

	ID3D11Device* device = Director::GetInstance()->GetDirectX()->GetDevice();
	HRESULT hr = device->CreateBuffer(&desc, &data, &_buffer);

	ASSERT_COND_MSG(SUCCEEDED( hr ), "Error!. does not create constant buffer");

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	srvDesc.BufferEx.FirstElement = 0;
	srvDesc.BufferEx.Flags = 0;
	srvDesc.BufferEx.NumElements = num;

	hr = device->CreateShaderResourceView(_buffer, &srvDesc, &_srv);
	ASSERT_COND_MSG(SUCCEEDED(hr), "Error!, does not create shader resource view");

	return true;
}