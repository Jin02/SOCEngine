#include "SpotLightShadow.h"
#include "SpotLight.h"
#include "Object.h"
#include "ShadowManager.h"
#include <assert.h>

using namespace Rendering::Shadow;
using namespace Rendering::Light;
using namespace Math;
using namespace Core;

SpotLightShadow::ViewProjMatType SpotLightShadow::MakeVPMatParam(
	const LightPool<SpotLight>& lightPool, const TransformPool& tfPool)
{		
	auto light = lightPool.Find(_base.GetObjectID().Literal());
	assert(light);

	const auto& lightBase = light->GetBase();

	auto transform = tfPool.Find(lightBase->GetObjectID().Literal());
	assert(transform);

	Matrix view = Matrix::ComputeViewMatrix(transform->GetWorldMatrix());

	float spotAngle	= DEG_2_RAD(light->GetSpotAngleDegree());
	float radius	= lightBase->GetRadius();
	float projNear	= _base.GetProjNear();

	Matrix proj				= Matrix::PerspectiveFovLH(1.0f, spotAngle, radius, projNear);
	_transposedViewProjMat	= Matrix::Transpose(view * proj);

	return _transposedViewProjMat;
}