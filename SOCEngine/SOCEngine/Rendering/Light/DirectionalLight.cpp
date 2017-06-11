#include "DirectionalLight.h"
#include "Object.hpp"
#include <assert.h>

using namespace Rendering;
using namespace Rendering::Light;
using namespace Math;
using namespace Core;

void DirectionalLight::UpdateFlag(const Transform& transform)
{
	assert(transform.GetObjectId() == _base.GetObjectId());

	bool isNegDirZSign = transform.GetWorldMatrix()._33 < 0.0f;
	_base.SetFlag( (_base.GetFlag() & 0xfe) | (isNegDirZSign ? 1 : 0) );
}

/*
void DirectionalLight::ComputeViewProjMatrix(const Transform& transform, const Intersection::BoundBox& sceneBoundBox, const Math::Matrix& invViewportMat)
{
	assert(transform.GetObjectId() == _base.GetObjectId());

#if 0
	Matrix view = transform.GetWorldMatrix();

	if(_useAutoProjectLocation)
	{
		Vector3 forward = Vector3(view._13, view._23, view._33).Normalized();
		const Vector3& sceneCenter = sceneBoundBox.GetCenter();

		view._41 = sceneCenter.x - (forward.x * DIRECTIONAL_LIGHT_FRUSTUM_MAX_Z / 2.0f);
		view._42 = sceneCenter.y - (forward.y * DIRECTIONAL_LIGHT_FRUSTUM_MAX_Z / 2.0f);
		view._43 = sceneCenter.z - (forward.z * DIRECTIONAL_LIGHT_FRUSTUM_MAX_Z / 2.0f);
	}

	CameraForm::GetViewMatrix(view, view);

	float orthogonalWH = (_projectionSize < FLT_EPSILON) ? sceneBoundBox.GetSize().Length() : _projectionSize;
#else
	Matrix view = transform.GetWorldMatrix();
	Vector3 forward = transform.GetWorldForward();//Vector3(view._13, view._23, view._33).Normalized();
	const Vector3& sceneCenter = sceneBoundBox.GetCenter();

	view._41 = sceneCenter.x - (forward.x * DIRECTIONAL_LIGHT_FRUSTUM_MAX_Z / 2.0f);
	view._42 = sceneCenter.y - (forward.y * DIRECTIONAL_LIGHT_FRUSTUM_MAX_Z / 2.0f);
	view._43 = sceneCenter.z - (forward.z * DIRECTIONAL_LIGHT_FRUSTUM_MAX_Z / 2.0f);

	MeshCamera::ComputeViewMatrix(view, view);

	float orthogonalWH = sceneBoundBox.GetSize().Length();
#endif
	Matrix proj;
	Matrix::OrthoLH(proj, orthogonalWH, orthogonalWH, DIRECTIONAL_LIGHT_FRUSTUM_MAX_Z, DIRECTIONAL_LIGHT_FRUSTUM_MIN_Z);

	_viewProjMat			= view * proj;

	Matrix::Inverse(proj, proj);
	_param.invProj_34 = proj._34;
	_param.invProj_44 = proj._44;
}
*/

DirectionalLight::TransformType DirectionalLight::MakeTransform(const Transform& transform) const
{
	assert(transform.GetObjectId() == _base.GetObjectId());

	const auto& forward = transform.GetForward();
	return TransformType(Half(forward.x), Half(forward.y));
}
