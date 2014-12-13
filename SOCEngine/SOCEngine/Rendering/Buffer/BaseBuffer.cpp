#include "BaseBuffer.h"
#include "Director.h"

using namespace Rendering::Buffer;
using namespace Device;

BaseBuffer::BaseBuffer() : _buffer(nullptr)
{
}

BaseBuffer::~BaseBuffer()
{
	SAFE_RELEASE(_buffer);
}

void BaseBuffer::Update(ID3D11DeviceContext* context, const void* data)
{
	context->UpdateSubresource(_buffer, 0, nullptr, data, 0, 0);
}