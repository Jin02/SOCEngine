#include "PointLightingBuffer.h"

using namespace Rendering;
using namespace Rendering::Light;
using namespace Rendering::Light::LightingBuffer;

PointLightingBuffer::PointLightingBuffer() : Parent()
{
}

PointLightingBuffer::~PointLightingBuffer()
{
}

TransformBuffer* PointLightingBuffer::InitializeTransformBuffer(uint maxLightCount)
{
	const __int32 dummy[maxLightCount * sizeof(Parent::TransformType) / 4] = {0, };
	return TransformBuffer::Create(maxLightCount, DXGI_FORMAT_R32G32B32A32_FLOAT, dummy);
}
