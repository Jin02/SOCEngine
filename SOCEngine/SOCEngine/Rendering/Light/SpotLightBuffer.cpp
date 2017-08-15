#include "SpotLightBuffer.h"

using namespace Rendering::Light;
using namespace Rendering::Light::Buffer;

inline void Rendering::Light::Buffer::SpotLightBuffer::Initialize(Device::DirectX & dx)
{
	Parent::Initialize(dx, POINT_LIGHT_BUFFER_MAX_NUM, DXGI_FORMAT_R32G32B32A32_FLOAT);

	SpotLight::Param dummy[POINT_LIGHT_BUFFER_MAX_NUM];
	_paramSRBuffer.Initialize(dx, POINT_LIGHT_BUFFER_MAX_NUM, DXGI_FORMAT_R16G16B16A16_FLOAT, dummy);
}

void SpotLightBuffer::PushLight(const SpotLight& light)
{
	_paramSRBuffer.PushData(SpotLight::Param());
	Parent::PushLight(light);
}

void SpotLightBuffer::UpdateTransformBuffer(	const std::vector<SpotLight*>& dirtyTFLights,
												const Core::TransformPool& tfPool,
												const Core::ObjectId::IndexHashMap& indexer)
{
	for (const auto& light : dirtyTFLights)
	{
		Core::ObjectId objId = light->GetObjectId();
		const auto& tf = tfPool.Find(objId.Literal());

		uint index = indexer.Find(objId.Literal());
		_transformBuffer[index] = light->MakeTransform(*tf);
		_paramSRBuffer[index] = light->MakeParam(*tf);
	}

	_mustUpdateTransformSRBuffer |= (dirtyTFLights.empty() != false);
}

void SpotLightBuffer::UpdateSRBuffer(Device::DirectX & dx)
{
	if (_mustUpdateTransformSRBuffer)
		_paramSRBuffer.UpdateSRBuffer(dx);

	Parent::UpdateSRBuffer(dx);
}

void SpotLightBuffer::Delete(uint index)
{
	_paramSRBuffer.Delete(index);
	Parent::Delete(index);
}

void SpotLightBuffer::DeleteAll()
{
	_paramSRBuffer.DeleteAll();
	Parent::DeleteAll();
}
