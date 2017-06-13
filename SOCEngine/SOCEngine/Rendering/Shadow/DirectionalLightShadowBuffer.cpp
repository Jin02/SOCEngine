#include "DirectionalLightShadowBuffer.h"

using namespace Rendering;
using namespace Rendering::Shadow;
using namespace Rendering::Shadow::Buffer;
using namespace Core;

void DirectionalLightShadowBuffer::Initialize(Device::DirectX & dx)
{
	Parent::Initialize(dx, DIRECTIONAL_LIGHT_BUFFER_MAX_NUM);

	DirectionalLightShadow::Param dummy[DIRECTIONAL_LIGHT_BUFFER_MAX_NUM];
	memset(dummy, 0, sizeof(dummy));
	_dlParamBuffer.Initialize(dx, DIRECTIONAL_LIGHT_BUFFER_MAX_NUM, DXGI_FORMAT_R16G16_FLOAT, dummy);

	_mustUpdateDLParamSRBuffer = true;
}

void DirectionalLightShadowBuffer::AddShadow(DirectionalLightShadow & shadow)
{
	uint id = shadow.GetShadowId().Literal();

	Parent::AddShadow(shadow);
	_dlParamBuffer.AddData(id, shadow.GetParam());

	_mustUpdateDLParamSRBuffer = true;
}

void DirectionalLightShadowBuffer::UpdateBuffer(
	const std::vector<DirectionalLightShadow*>& dirtyShadows,
	const Light::LightPool<Light::DirectionalLight>& lightPool,
	const Core::TransformPool& tfPool,
	const Intersection::BoundBox& sceneBoundBox)
{
	for (auto& shadow : dirtyShadows)
	{
		auto& base = shadow->GetBase();
		uint id = base.GetShadowId().Literal();

		_paramBuffer.SetData(id, base.GetParam());
		_tfBuffer.SetData(id, shadow->MakeVPMatParam(lightPool, tfPool, sceneBoundBox));
		_dlParamBuffer.SetData(id, shadow->GetParam());
	}

	_mustUpdateParamSRBuffer |= (dirtyShadows.empty() != false);
	_mustUpdateTransformSRBuffer |= _mustUpdateParamSRBuffer;
	_mustUpdateDLParamSRBuffer |= _mustUpdateParamSRBuffer;
}

void DirectionalLightShadowBuffer::UpdateSRBuffer(Device::DirectX & dx)
{
	if (_mustUpdateDLParamSRBuffer)
		_dlParamBuffer.UpdateSRBuffer(dx);

	Parent::UpdateSRBuffer(dx);

	_mustUpdateDLParamSRBuffer = false;
}

void DirectionalLightShadowBuffer::Delete(const DirectionalLightShadow & shadow)
{
	uint id = shadow.GetShadowId().Literal();

	_dlParamBuffer.Delete(id);
	Parent::Delete(shadow);

	_mustUpdateDLParamSRBuffer = true;
}

void DirectionalLightShadowBuffer::DeleteAll()
{
	_dlParamBuffer.DeleteAll();
	Parent::DeleteAll();

	_mustUpdateDLParamSRBuffer = true;
}
