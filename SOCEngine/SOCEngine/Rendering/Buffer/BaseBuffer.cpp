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