#include "ConstBuffer.h"
#include "Director.h"
#include <type_traits> 

using namespace Rendering::Buffer;
using namespace Device;

ConstBuffer::ConstBuffer() : BaseBuffer()
{
}

ConstBuffer::~ConstBuffer()
{
}

bool ConstBuffer::Create(unsigned int size)
{
	D3D11_BUFFER_DESC desc;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = size;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	ID3D11Device* device = Director::GetInstance()->GetDirectX()->GetDevice();
	HRESULT hr = device->CreateBuffer(&desc, nullptr, &_buffer);

	ASSERT_COND_MSG(SUCCEEDED(hr), "Error!. Not create constant buffer");
	return true;
}