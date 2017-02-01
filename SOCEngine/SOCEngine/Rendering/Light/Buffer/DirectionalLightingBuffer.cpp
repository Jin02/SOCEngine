#include "DirectionalLightingBuffer.h"

using namespace Rendering;
using namespace Rendering::Light;
using namespace Rendering::Light::LightingBuffer;

DirectionalLightingBuffer::DirectionalLightingBuffer() : Parent()
{
}

DirectionalLightingBuffer::~DirectionalLightingBuffer()
{
}

TransformBuffer* DirectionalLightingBuffer::InitializeTransformBuffer(uint maxLightCount)
{
	const __int32 dummy[maxLightCount * sizeof(Parent::TransformType) / 4] = {0, };
	return TransformBuffer::Create(maxLightCount, DXGI_FORMAT_R16G16_FLOAT, dummy);
}

DirectionalLightingBuffer* DirectionalLightingBuffer::Create()
{
	DirectionalLightingBuffer* buff = new DirectionalLightingBuffer;
	buff->Initialize();
	
	return buff;
}
