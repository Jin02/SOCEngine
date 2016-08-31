#include "ShaderResourceBuffer.h"
#include "Director.h"

using namespace Device;
using namespace Rendering::Buffer;

ShaderResourceBuffer::ShaderResourceBuffer() : BaseBuffer(), _srv(nullptr)
{

}

ShaderResourceBuffer::~ShaderResourceBuffer()
{
	_srv->Destroy();

	SAFE_DELETE(_srv);
	SAFE_RELEASE(_buffer);
}

void ShaderResourceBuffer::Initialize(
	uint stride, uint num, DXGI_FORMAT format,
	const void* sysMem, bool useMapWriteNoOverWrite,
	uint optionalBindFlag, D3D11_USAGE usage, D3D11_RESOURCE_MISC_FLAG miscFlag)
{
	D3D11_BUFFER_DESC desc;
	memset(&desc, 0, sizeof(D3D11_BUFFER_DESC));

	desc.Usage					= usage;
	desc.ByteWidth				= stride * num;
	desc.BindFlags				= D3D11_BIND_SHADER_RESOURCE | optionalBindFlag;
	desc.CPUAccessFlags			= useMapWriteNoOverWrite ? D3D11_CPU_ACCESS_WRITE : 0;
	desc.StructureByteStride		= stride;

	//아래 구문 수정해야함.
	//misc를 처음부터 받아와서 구현하면 되지만, 어디서 수정되었는지를 현 상황에서 하나하나 찾기 힘듬..
	if(miscFlag == 0)		desc.MiscFlags = (format == DXGI_FORMAT_UNKNOWN) ? D3D11_RESOURCE_MISC_BUFFER_STRUCTURED : 0;
	else				desc.MiscFlags = miscFlag;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = sysMem;

	ID3D11Device* device = Director::SharedInstance()->GetDirectX()->GetDevice();
	HRESULT hr = sysMem ? device->CreateBuffer(&desc, &data, &_buffer)
						: device->CreateBuffer(&desc, nullptr, &_buffer);

	ASSERT_COND_MSG(SUCCEEDED( hr ), "Error!. can't create buffer");
	
	if(_srv == nullptr)	_srv = new ShaderResourceView;
	_srv->Initialize(_buffer, format, 0, D3D11_SRV_DIMENSION_BUFFER, num, stride);
}
