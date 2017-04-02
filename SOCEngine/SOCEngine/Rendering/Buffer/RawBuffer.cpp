#include "RawBuffer.h"
#include <assert.h>

using namespace Rendering::View;
using namespace Rendering::Buffer;
using namespace Rendering::Shader;
using namespace Device;

void RawBuffer::Initialize(Device::DirectX& dx, uint stride, uint elemNum, Flag flag)
{
	bool useAll = flag == Flag::ALL_VIEW;
	bool useSRV = (flag == Flag::ONLY_SRV) | useAll;
	bool useUAV = (flag == Flag::ONLY_UAV) | useAll;

	uint bindFlag = (useSRV ? D3D11_BIND_SHADER_RESOURCE : 0) | (useUAV ? D3D11_BIND_UNORDERED_ACCESS : 0);

	D3D11_BUFFER_DESC desc;
	memset(&desc, 0, sizeof(D3D11_BUFFER_DESC));

	desc.Usage					= D3D11_USAGE_DEFAULT;
	desc.ByteWidth				= stride * elemNum;
	desc.BindFlags				= bindFlag;
	desc.StructureByteStride	= stride;
	desc.MiscFlags				= D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;

	_baseBuffer.SetBuffer(dx.CreateBuffer(desc, nullptr));

	if(useSRV)	_srv.InitializeUsingBuffer(dx, GetBaseBuffer(), elemNum, DXGI_FORMAT_R32_UINT, true);
	if(useUAV)	_uav.Initialize(dx, DXGI_FORMAT_R32_TYPELESS, elemNum, GetBaseBuffer().GetBuffer(), D3D11_UAV_DIMENSION_BUFFER, 0, 0, D3D11_BUFFER_UAV_FLAG_RAW);
}
