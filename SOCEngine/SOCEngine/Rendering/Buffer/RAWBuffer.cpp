#include "RAWBuffer.h"
#include "Director.h"

using namespace Rendering::View;
using namespace Rendering::Buffer;
using namespace Rendering::Shader;
using namespace Device;

RAWBuffer::RAWBuffer() : ShaderResourceBuffer(), _uav(nullptr)
{
}

RAWBuffer::~RAWBuffer()
{
	Destroy();
	SAFE_DELETE(_uav);
}

bool RAWBuffer::Initialize(uint stride, uint num)
{
	ShaderResourceBuffer::Initialize(stride, num,
	DXGI_FORMAT_R32_UINT, nullptr, false,
	D3D11_BIND_UNORDERED_ACCESS, D3D11_USAGE_DEFAULT, D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS);

	if(_uav == nullptr)	_uav = new UnorderedAccessView;
	_uav->Initialize(DXGI_FORMAT_R32_TYPELESS, num, _buffer, D3D11_UAV_DIMENSION_BUFFER, 0, 0, D3D11_BUFFER_UAV_FLAG_RAW);

	return true;
}

void RAWBuffer::Destroy()
{
	_srv->Destroy();
	_uav->Destroy();
}
