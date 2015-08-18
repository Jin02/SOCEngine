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
	SAFE_RELEASE(_buffer);
}

void CSInputBuffer::Initialize(uint stride, uint num, DXGI_FORMAT format, const void* sysMem)
{
	D3D11_BUFFER_DESC desc;
	memset(&desc, 0, sizeof(D3D11_BUFFER_DESC));

	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.ByteWidth = stride * num;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = sysMem;

	ID3D11Device* device = Director::GetInstance()->GetDirectX()->GetDevice();
	HRESULT hr = device->CreateBuffer(&desc, &data, &_buffer);

	ASSERT_COND_MSG(SUCCEEDED( hr ), "Error!. does not create constant buffer");

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.ElementOffset = 0;
	srvDesc.Buffer.ElementWidth = num;

	hr = device->CreateShaderResourceView(_buffer, &srvDesc, &_srv);
	ASSERT_COND_MSG(SUCCEEDED(hr), "Error!, does not create shader resource view");
}