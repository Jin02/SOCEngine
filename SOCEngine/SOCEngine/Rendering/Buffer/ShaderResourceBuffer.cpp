#include "ShaderResourceBuffer.h"
#include "DirectX.h"

using namespace Device;
using namespace Rendering::Buffer;
using namespace Rendering::View;

ShaderResourceBuffer::ShaderResourceBuffer(const BaseBuffer& buf, const ShaderResourceView& srv)
	: _baseBuffer(buf), _srv(srv)
{
}

void ShaderResourceBuffer::Initialize(
	Device::DirectX& dx,
	uint stride, uint num, DXGI_FORMAT format,
	const void* sysMem, uint optionalBindFlag, D3D11_USAGE usage)
{
	D3D11_BUFFER_DESC desc;
	memset(&desc, 0, sizeof(D3D11_BUFFER_DESC));

	desc.Usage					= usage;
	desc.ByteWidth				= stride * num;
	desc.BindFlags				= D3D11_BIND_SHADER_RESOURCE | optionalBindFlag;
	desc.CPUAccessFlags			= (usage == D3D11_USAGE_DYNAMIC) ? D3D11_CPU_ACCESS_WRITE : 0;
	desc.StructureByteStride	= stride;
	desc.MiscFlags				= (format == DXGI_FORMAT_UNKNOWN) ? D3D11_RESOURCE_MISC_BUFFER_STRUCTURED : 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = sysMem;

	_baseBuffer.SetBuffer(dx.CreateBuffer(desc, sysMem ? &data : nullptr));
	_srv.InitializeUsingBuffer(dx, _baseBuffer, num, format, false);
}

void ShaderResourceBuffer::Destroy()
{
	_srv.Destroy();
	_baseBuffer.Destroy();
}
