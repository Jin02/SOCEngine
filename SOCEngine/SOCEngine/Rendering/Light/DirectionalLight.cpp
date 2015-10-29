#include "DirectionalLight.h"
#include "Object.h"
#include "CameraForm.h"

using namespace Rendering;
using namespace Rendering::Camera;
using namespace Rendering::Light;
using namespace Math;
using namespace Core;

DirectionalLight::DirectionalLight() : LightForm()
{
	_type = LightType::Directional;
}

DirectionalLight::~DirectionalLight()
{
}

void DirectionalLight::ComputeViewProjMatrix(const Intersection::BoundBox& sceneBoundBox)
{
	Matrix& view = _viewMat;
	_owner->GetTransform()->FetchWorldMatrix(view);

	Vector3 forward = Vector3(view._13, view._23, view._33).Normalize();
	const Vector3& sceneCenter = sceneBoundBox.GetCenter();

	view._41 = sceneCenter.x - (forward.x * DIRECTIONAL_LIGHT_FRUSTUM_MAX_Z / 2.0f);
	view._42 = sceneCenter.y - (forward.y * DIRECTIONAL_LIGHT_FRUSTUM_MAX_Z / 2.0f);
	view._43 = sceneCenter.z - (forward.z * DIRECTIONAL_LIGHT_FRUSTUM_MAX_Z / 2.0f);

	CameraForm::GetViewMatrix(view, view);

	float orthogonalWH	= sceneBoundBox.GetSize().Length();

	Matrix proj;
	Matrix::OrthoLH(proj, orthogonalWH, orthogonalWH, DIRECTIONAL_LIGHT_FRUSTUM_MAX_Z, DIRECTIONAL_LIGHT_FRUSTUM_MIN_Z); //inverted

	Matrix& viewProj = _viewProjMat;
	viewProj = view * proj;

	//°â»ç°â»ç Frustumµµ °è»ê
	Matrix notInvertedProj;
	Matrix::OrthoLH(notInvertedProj, orthogonalWH, orthogonalWH, DIRECTIONAL_LIGHT_FRUSTUM_MIN_Z, DIRECTIONAL_LIGHT_FRUSTUM_MAX_Z);
	_frustum.Make(view * notInvertedProj);
}

bool DirectionalLight::Intersect(const Intersection::Sphere &sphere) const
{
	return _frustum.In(sphere.center, sphere.radius);
}

void DirectionalLight::MakeLightBufferElement(LightTransformBuffer& outTransform, Params& outParam) const
{
	const Transform* transform = _owner->GetTransform();
	Transform worldTransform(nullptr);
	transform->FetchWorldTransform(worldTransform);
	outTransform.worldPosition = worldTransform.GetLocalPosition();

	const auto& forward = worldTransform.GetForward();
	outTransform.radius = forward.z;
	
	outParam.dirX = Math::Common::FloatToHalf(forward.x);
	outParam.dirY = Math::Common::FloatToHalf(forward.y);
}

Core::Component* DirectionalLight::Clone() const
{
	return new DirectionalLight(*this);
}