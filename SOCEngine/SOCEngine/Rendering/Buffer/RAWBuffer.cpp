#include "RAWBuffer.h"
#include "Director.h"

using namespace Rendering::View;
using namespace Rendering::Buffer;
using namespace Rendering::Shader;
using namespace Device;

RAWBuffer::RAWBuffer() : BaseBuffer(), _srv(nullptr), _uav(nullptr)
{
}

RAWBuffer::~RAWBuffer()
{
	Destroy();

	SAFE_DELETE(_srv);
	SAFE_DELETE(_uav);
}

bool RAWBuffer::Initialize(uint stride, uint num)
{
	D3D11_BUFFER_DESC desc;
	memset(&desc, 0, sizeof(D3D11_BUFFER_DESC));
	desc.BindFlags				= D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	desc.MiscFlags				= D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
	desc.StructureByteStride	= stride;
	desc.ByteWidth				= stride * num;

	ID3D11Device* device = Director::SharedInstance()->GetDirectX()->GetDevice();
	HRESULT hr = device->CreateBuffer(&desc, nullptr, &_buffer);

	ASSERT_COND_MSG(SUCCEEDED(hr), "Error!. Not create buffer");

	if(_srv == nullptr)	_srv = new ShaderResourceView;
	_srv->Initialize(_buffer, DXGI_FORMAT_R32_UINT, 0, D3D11_SRV_DIMENSION_BUFFER, num);

	if(_uav == nullptr)	_uav = new UnorderedAccessView;
	_uav->Initialize(DXGI_FORMAT_R32_TYPELESS, num, _buffer, D3D11_UAV_DIMENSION_BUFFER, 0, 0, D3D11_BUFFER_UAV_FLAG_RAW);

	return true;
}

void RAWBuffer::Destroy()
{
	_srv->Destory();
	_uav->Destroy();
}