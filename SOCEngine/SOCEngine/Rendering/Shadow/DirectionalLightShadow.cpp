#include "DirectionalLightShadow.h"
#include "DirectionalLight.h"
#include "ShadowManager.h"
#include <assert.h>

using namespace Rendering::Shadow;
using namespace Rendering::Light;
using namespace Math;
using namespace Core;
using namespace Intersection;

DirectionalLightShadow::ViewProjMatType DirectionalLightShadow::MakeVPMatParam(
	const LightPool<DirectionalLight>& lightPool,
	const TransformPool& tfPool,
	const BoundBox& sceneBoundBox)
{
	auto light = lightPool.Find(_base.GetObjectID().Literal());
	assert(light);

	const auto& lightBase = light->GetBase();

	auto transform = tfPool.Find(lightBase->GetObjectID().Literal());
	assert(transform);

	Matrix view = transform->GetWorldMatrix();

	if (_useAutoProjectLocation)
	{
		Vector3 forward = transform->GetWorldForward();
		const Vector3& sceneCenter = sceneBoundBox.GetCenter();

		view._41 = sceneCenter.x - (forward.x * FrustumMaxZ / 2.0f);
		view._42 = sceneCenter.y - (forward.y * FrustumMaxZ / 2.0f);
		view._43 = sceneCenter.z - (forward.z * FrustumMaxZ / 2.0f);
		view._44 = 1.0f;
	}

	view = Matrix::ComputeViewMatrix(view);

	float orthogonalWH	= (_projectionSize < FLT_EPSILON) ? sceneBoundBox.GetSize().Length() : _projectionSize;
	Matrix proj			= Matrix::OrthoLH(orthogonalWH, orthogonalWH, FrustumMaxZ, FrustumMinZ);

	// Done!
	_transposedViewProjMat = Matrix::Transpose(view * proj);
	return _transposedViewProjMat;
}
