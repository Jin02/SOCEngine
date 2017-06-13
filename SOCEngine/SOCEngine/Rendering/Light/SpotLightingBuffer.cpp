#include "SpotLightingBuffer.h"

using namespace Rendering::Light;
using namespace Rendering::Light::LightingBuffer;

inline void Rendering::Light::LightingBuffer::SpotLightingBuffer::Initialize(Device::DirectX & dx)
{
	Parent::Initialize(dx, POINT_LIGHT_BUFFER_MAX_NUM, DXGI_FORMAT_R32G32B32A32_FLOAT);

	SpotLight::Param dummy[POINT_LIGHT_BUFFER_MAX_NUM];
	_paramSRBuffer.Initialize(dx, POINT_LIGHT_BUFFER_MAX_NUM, DXGI_FORMAT_R16G16B16A16_FLOAT, dummy);
}

void SpotLightingBuffer::AddLight(const SpotLight& light)
{
	_paramSRBuffer.AddData(light.GetLightId().Literal(), SpotLight::Param());
	Parent::AddLight(light);
}

void SpotLightingBuffer::UpdateTransformBuffer(	const std::vector<SpotLight*>& dirtyTFLights,
												const Core::TransformPool& tfPool)
{
	for (const auto& light : dirtyTFLights)
	{
		Core::ObjectId objId = light->GetObjectId();
		const auto& tf = tfPool.Find(objId.Literal());

		uint lightId = light->GetBase().GetLightId().Literal();
		_transformBuffer.SetData(lightId, light->MakeTransform(*tf));
		_paramSRBuffer.SetData(lightId, light->MakeParam(*tf));
	}

	_mustUpdateTransformSRBuffer |= (dirtyTFLights.empty() != false);
}

void SpotLightingBuffer::UpdateSRBuffer(Device::DirectX & dx)
{
	if (_mustUpdateTransformSRBuffer)
		_paramSRBuffer.UpdateSRBuffer(dx);

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
