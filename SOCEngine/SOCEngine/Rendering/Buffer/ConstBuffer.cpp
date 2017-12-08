#include "ConstBuffer.h"
#include "DirectX.h"

using namespace Rendering::Buffer;

void ConstBuffer::Initialize(Device::DirectX& dx, uint size)
{
	assert(_base.GetBuffer().IsCanUse() == false);
	_size = size;

	D3D11_BUFFER_DESC desc;
	desc.Usage					= D3D11_USAGE_DEFAULT;
	desc.ByteWidth				= size;
	desc.BindFlags				= D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags			= 0;
	desc.MiscFlags				= 0;
	desc.StructureByteStride	= 0;

	_base.SetBuffer(dx.CreateBuffer(desc, nullptr));
}

void ConstBuffer::UpdateSubResource(Device::DirectX& dx, const void* data)
{
	_base.UpdateSubResource(dx, data);
}
