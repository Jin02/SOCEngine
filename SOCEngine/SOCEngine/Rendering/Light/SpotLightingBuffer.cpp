#include "SpotLightingBuffer.h"

using namespace Rendering::Light;
using namespace Rendering::Light::LightingBuffer;

void SpotLightingBuffer::AddLight(const SpotLight & light, const Core::Transform & lightTransform, const RequiredIndexBook& indexBooks)
{
	assert(light.GetObjectId() == lightTransform.GetObjectId());

	_paramSRBuffer.AddData(light.GetLightId().Literal(), light.MakeParam(lightTransform));
	Parent::AddLight(light, lightTransform, indexBooks);
}

void SpotLightingBuffer::UpdateParamBuffer(const std::vector<SpotLight*>& dirtyLights,
											const Core::TransformPool & transformPool)
{
	for (auto& light : dirtyLights)
	{
		uint lightId = light->GetLightId().Literal();

		const auto& tf = transformPool.Find(light->GetObjectId().Literal());
		assert(tf);

		_paramSRBuffer.SetData(lightId, light->MakeParam(*tf));
	}

	_mustUpdateParamSRBuffer |= (dirtyLights.empty() != false);
}

void SpotLightingBuffer::UpdateSRBuffer(Device::DirectX & dx)
{
	if (_mustUpdateParamSRBuffer)
		_paramSRBuffer.UpdateSRBuffer(dx);

	_mustUpdateParamSRBuffer = false;

	Parent::UpdateSRBuffer(dx);
}

void SpotLightingBuffer::Delete(const SpotLight & light)
{
	_paramSRBuffer.Delete(light.GetObjectId().Literal());
	Parent::Delete(light);
}

void SpotLightingBuffer::DeleteAll()
{
	_paramSRBuffer.DeleteAll();
	Parent::DeleteAll();
}
