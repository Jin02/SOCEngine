#include "ShaderResourceBuffer.h"
#include "Director.h"

using namespace Device;
using namespace Rendering::Buffer;

ShaderResourceBuffer::ShaderResourceBuffer() : BaseBuffer(), _srv(nullptr)
{

}

ShaderResourceBuffer::~ShaderResourceBuffer()
{
	SAFE_RELEASE(_srv);
	SAFE_RELEASE(_buffer);
}

void ShaderResourceBuffer::Initialize(
	uint stride, uint num, DXGI_FORMAT format,
	const void* sysMem, bool useMapWriteNoOverWrite,
	uint optionalBindFlag, D3D11_USAGE usage)
{
	D3D11_BUFFER_DESC desc;
	memset(&desc, 0, sizeof(D3D11_BUFFER_DESC));

	desc.Usage					= usage;
	desc.ByteWidth				= stride * num;
	desc.BindFlags				= D3D11_BIND_SHADER_RESOURCE | optionalBindFlag;
	desc.CPUAccessFlags			= useMapWriteNoOverWrite ? D3D11_CPU_ACCESS_WRITE : 0;
	desc.StructureByteStride	= stride;
//	desc.StructureByteStride	= optionalBindFlag & D3D11_BIND_UNORDERED_ACCESS ? 0 : stride;
	desc.MiscFlags				= (format == DXGI_FORMAT_UNKNOWN) ? D3D11_RESOURCE_MISC_BUFFER_STRUCTURED : 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = sysMem;

	ID3D11Device* device = Director::SharedInstance()->GetDirectX()->GetDevice();
	HRESULT hr = sysMem ? device->CreateBuffer(&desc, &data, &_buffer)
						: device->CreateBuffer(&desc, nullptr, &_buffer);

	ASSERT_COND_MSG(SUCCEEDED( hr ), "Error!. can't create buffer");

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.ElementOffset = 0;
	srvDesc.Buffer.ElementWidth = num;

	hr = device->CreateShaderResourceView(_buffer, &srvDesc, &_srv);
	ASSERT_COND_MSG(SUCCEEDED(hr), "Error!, does not create shader resource view");
}