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

void DirectionalLightShadowBuffer::PushShadow(DirectionalLightShadow & shadow)
{
	Parent::PushShadow(shadow);
	_dlParamBuffer.PushData(shadow.GetParamCBData());

	_mustUpdateDLParamSRBuffer = true;
}

void DirectionalLightShadowBuffer::UpdateBuffer(
	const std::vector<DirectionalLightShadow*>& dirtyShadows,
	const Light::LightPool<Light::DirectionalLight>& lightPool,
	const Core::TransformPool& tfPool,
	const ShadowDatasIndexer& indexer,
	const Intersection::BoundBox& sceneBoundBox)
{
	for (auto& shadow : dirtyShadows)
	{
		auto& base = shadow->GetBase();
		Core::ObjectID objID = base.GetObjectID();

		uint index = indexer.Find(objID.Literal());
		_paramBuffer[index]				= base.GetParam();
		_paramBuffer[index].lightIndex	= lightPool.GetIndexer().Find(objID.Literal());
		_transformBuffer[index]			= shadow->MakeVPMatParam(lightPool, tfPool, sceneBoundBox);
		_dlParamBuffer[index]			= shadow->GetParam();
	}

	_mustUpdateParamSRBuffer		|= (dirtyShadows.empty() == false);
	_mustUpdateTransformSRBuffer	|= _mustUpdateParamSRBuffer;
	_mustUpdateDLParamSRBuffer		|= _mustUpdateParamSRBuffer;
}

void DirectionalLightShadowBuffer::UpdateSRBuffer(Device::DirectX & dx, bool forcedUpdate)
{
	if (_mustUpdateDLParamSRBuffer | forcedUpdate)
		_dlParamBuffer.UpdateSRBuffer(dx);

	Parent::UpdateSRBuffer(dx, forcedUpdate);

	_mustUpdateDLParamSRBuffer = false;
}

void DirectionalLightShadowBuffer::Delete(uint index)
{
	_dlParamBuffer.Delete(index);
	Parent::Delete(index);

	_mustUpdateDLParamSRBuffer = true;
}

void DirectionalLightShadowBuffer::DeleteAll()
{
	_dlParamBuffer.DeleteAll();
	Parent::DeleteAll();

	_mustUpdateDLParamSRBuffer = true;
}
