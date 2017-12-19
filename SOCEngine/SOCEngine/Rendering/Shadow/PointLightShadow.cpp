#include "PointLightShadow.h"
#include "PointLight.h"
#include "Object.h"
#include "ShadowManager.h"
#include "PointLightShadowMapCBPool.h"
#include <assert.h>

using namespace Rendering::Shadow;
using namespace Rendering::Light;
using namespace Math;
using namespace Core;

PointLightShadow::ViewProjMatType PointLightShadow::MakeVPMatParam(const LightPool<PointLight>& plPool, const TransformPool& tfPool)
{
	auto light = plPool.Find(_base.GetObjectID().Literal());
	assert(light); // error! light is null

	const auto& lightBase = light->GetBase();

	auto transform = tfPool.Find(lightBase->GetObjectID().Literal());
	assert(transform);


	Vector3 forwards[6] = 
	{
		Vector3( 0.0f,  0.0f,  1.0f),
		Vector3( 0.0f,  0.0f, -1.0f),
		Vector3( 1.0f,  0.0f,  0.0f),
		Vector3(-1.0f,  0.0f,  0.0f),
		Vector3( 0.0f,  1.0f,  0.0f),
		Vector3( 0.0f, -1.0f,  0.0f)
	};
	Vector3 ups[6] = 
	{
		Vector3( 0.0f,  1.0f,  0.0f),
		Vector3( 0.0f,  1.0f,  0.0f),
		Vector3( 0.0f,  1.0f,  0.0f),
		Vector3( 0.0f,  1.0f,  0.0f),
		Vector3( 0.0f,  0.0f, -1.0f),
		Vector3( 0.0f,  0.0f,  1.0f),
	};


	float radius		= lightBase->GetRadius();
	float projNear		= _base.GetProjNear();
	Matrix proj			= Matrix::PerspectiveFovLH(1.0f, DEG_2_RAD(90.0f), radius, projNear);
	Vector3 worldPos	= transform->GetWorldPosition();

	for (uint i = 0; i < 6; ++i)
		_transposedViewProjMat[i] = Matrix::Transpose( Matrix::ComputeViewProjMatrix(worldPos, forwards[i], ups[i], proj) );

	return _transposedViewProjMat;
}