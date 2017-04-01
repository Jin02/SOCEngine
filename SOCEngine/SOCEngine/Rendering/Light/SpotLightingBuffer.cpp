#include "SpotLightingBuffer.h"

using namespace Rendering::Light;
using namespace Rendering::Light::LightingBuffer;

void SpotLightingBuffer::Initialize(Device::DirectX & dx)
{
	_base.Initialize(dx, 2048, DXGI_FORMAT_R32G32B32A32_FLOAT);

	SpotLight::Param dummy[2048];
	_paramSRBuffer.Initialize(dx, 2048, DXGI_FORMAT_R16G16B16A16_FLOAT, dummy);
}

void SpotLightingBuffer::UpdateBuffer(std::vector<SpotLight>& lights,
	const std::function<uchar(const BaseLight&)>& getShadowIndex,
	const std::function<uchar(const BaseLight&)>& getLightShaftIndex,
	const Core::TransformPool & pool)
{
	std::function<void(SpotLight&, const Core::Transform&, bool)> func =
		[this](SpotLight& light, const Core::Transform& transform, bool existElem) -> void
	{
		if (light.GetDirty() == false)
			return;

		SpotLight::Param param;
		light.MakeParam(param, transform);

		auto key = light.GetObjectId();

		if (existElem)	(*_paramSRBuffer.Find(key)) = param;
		else			_paramSRBuffer.Add(key, param);

		light.SetDirty(false);
	};

	_base.UpdateBuffer(lights, getShadowIndex, getLightShaftIndex, pool, &func);
}

void SpotLightingBuffer::UpdateSRBuffer(Device::DirectX & dx)
{
	_base.UpdateSRBuffer(dx);
	_paramSRBuffer.UpdateSRBuffer(dx);

	_mustUpdateParamSRBuffer = false;
}

void SpotLightingBuffer::Delete(Core::ObjectId id)
{
	_base.Delete(id);
	_paramSRBuffer.Delete(id);
}

void SpotLightingBuffer::DeleteAll()
{
	_base.DeleteAll();
	_paramSRBuffer.DeleteAll();
}
