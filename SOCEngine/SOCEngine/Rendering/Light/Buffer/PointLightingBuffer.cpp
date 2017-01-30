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
	const __int32 dummy[maxLightCount * sizeof(LightForm::LightTransformBuffer) / 4] = {0, };
	return TransformBuffer::Create(maxLightCount, DXGI_FORMAT_R32G32B32A32_FLOAT, dummy);
}

bool PointLightingBuffer::UpdateBuffer(	const LightWithPrevUpdateCounter& lightWithPrevUC,
					const std::function<uchar(const LightForm*)>& getShadowIndex,
					const std::function<uchar(const LightForm*)>& getLightShaftIndex)
{
	auto _UpdateBuffer = [&](const LightForm* light) -> void
	{
		address key = reinterpret_cat<address>(light);
		
		LightForm::LightTransformBuffer	transform;
		static_cast<const PointLight*>(light)->MakeParam(transform);
		
		Parent::UpdateTransformBuffer(key, transform);
	};
	
	return lightWithPrevUC->UpdateBuffer(_UpdateBuffer);
}
