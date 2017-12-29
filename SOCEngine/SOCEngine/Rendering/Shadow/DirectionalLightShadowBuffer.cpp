#include "DirectionalLightShadowBuffer.h"

using namespace Rendering;
using namespace Rendering::Shadow;
using namespace Rendering::Shadow::Buffer;
using namespace Core;

void DirectionalLightShadowBuffer::UpdateBuffer(
	const std::vector<DirectionalLightShadow*>& dirtyShadows,
	const Light::LightPool<Light::DirectionalLight>& lightPool,
	const Core::TransformPool& tfPool,
	const ShadowDatasIndexer& indexer,
	const Intersection::BoundBox& sceneBoundBox)
{
	for (auto& shadow : dirtyShadows)
	{
		const auto& base = shadow->GetBase();
		Core::ObjectID objID = base->GetObjectID();

		uint index = indexer.Find(objID.Literal());
		_transformBuffer[index]			= shadow->MakeVPMatParam(lightPool, tfPool, sceneBoundBox);

		_paramBuffer[index]				= base->GetParam();
		_paramBuffer[index].projNear	= shadow->GetTransposedViewMat()._34;
		_paramBuffer[index].lightIndex	= lightPool.GetIndexer().Find(objID.Literal());
	}

	_mustUpdateParamSRBuffer		|= (dirtyShadows.empty() == false);
	_mustUpdateTransformSRBuffer	|= _mustUpdateParamSRBuffer;
}