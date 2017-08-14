#include "PointLightShadow.h"
#include "PointLight.h"
#include "Object.h"
#include "ShadowManager.h"
#include <assert.h>

using namespace Rendering::Shadow;
using namespace Rendering::Light;
using namespace Math;
using namespace Core;

PointLightShadow::ViewProjMatType PointLightShadow::MakeVPMatParam(const LightPool<PointLight>& plPool, const TransformPool& tfPool)
{
	assert(_base.GetDirty());

	auto light = plPool.Find(_base.GetObjectId().Literal());
	assert(light); // error! light is null

	const auto& lightBase = light->GetBase();

	auto transform = tfPool.Find(lightBase.GetObjectId().Literal());
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


	float radius	= lightBase.GetRadius();
	float projNear	= _base.GetProjNear();

	Matrix proj = Matrix::PerspectiveFovLH(1.0f, DEG_2_RAD(90.0f), radius, projNear);

	auto ComputeViewProj = [](const Vector3& eyePos, const Vector3& forward, const Vector3& up, const Matrix& projMat)
	{
		Matrix view = Matrix::LookAtDir((eyePos + forward).Normalized(), &up);
		{
			view._41 = eyePos.x;
			view._42 = eyePos.y;
			view._43 = eyePos.z;
			view._44 = 1.0f;

			view = Matrix::ComputeViewMatrix(view);
		}

		return view * projMat;
	};

	Vector3 worldPos = transform->GetWorldPosition();

	for (uint i = 0; i < 6; ++i)
		_transposedViewProjMat[i] = Matrix::Transpose( ComputeViewProj(worldPos, forwards[i], ups[i], proj) );

	return _transposedViewProjMat;
}