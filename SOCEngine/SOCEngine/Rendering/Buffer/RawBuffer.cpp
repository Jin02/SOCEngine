#include "RawBuffer.h"
#include "Director.h"

using namespace Rendering::View;
using namespace Rendering::Buffer;
using namespace Rendering::Shader;
using namespace Device;

RawBuffer::RawBuffer() : ShaderResourceBuffer(), _uav(nullptr)
{
}

RawBuffer::~RawBuffer()
{
	Destroy();
	SAFE_DELETE(_uav);
}

bool RawBuffer::Initialize(uint stride, uint elemNum, Flag flag)
{
	bool useAll = flag == Flag::ALL_VIEW;
	bool useSRV = flag == Flag::ONLY_SRV || useAll;
	bool useUAV = flag == Flag::ONLY_UAV || useAll;

	uint bindFlag = useSRV ? D3D11_BIND_SHADER_RESOURCE : 0 | useUAV ? D3D11_BIND_UNORDERED_ACCESS : 0;

	D3D11_BUFFER_DESC desc;
	memset(&desc, 0, sizeof(D3D11_BUFFER_DESC));

	desc.Usage					= D3D11_USAGE_DEFAULT;
	desc.ByteWidth				= stride * elemNum;
	desc.BindFlags				= bindFlag;
	desc.StructureByteStride	= stride;
	desc.MiscFlags				= D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;

	ID3D11Device* device = Director::SharedInstance()->GetDirectX()->GetDevice();
	HRESULT hr = device->CreateBuffer(&desc, nullptr, &_buffer);

	ASSERT_COND_MSG(SUCCEEDED( hr ), "Error!. can't create buffer");

	if(useSRV)
	{
		if(_srv == nullptr)	_srv = new View::ShaderResourceView;
		_srv->InitializeUsingBuffer(_buffer, elemNum, DXGI_FORMAT_R32_UINT);
	}

	if(useUAV)
	{
		if(_uav == nullptr)	_uav = new UnorderedAccessView;
		_uav->Initialize(DXGI_FORMAT_R32_TYPELESS, elemNum, _buffer, D3D11_UAV_DIMENSION_BUFFER, 0, 0, D3D11_BUFFER_UAV_FLAG_RAW);
	}

	return true;
}

void RawBuffer::Destroy()
{
	ShaderResourceBuffer::Destroy();
	_uav->Destroy();
}
