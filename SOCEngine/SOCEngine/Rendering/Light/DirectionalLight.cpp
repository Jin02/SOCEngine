#include "DirectionalLight.h"
#include "Object.h"
#include "CameraForm.h"

using namespace Rendering;
using namespace Rendering::Camera;
using namespace Rendering::Shadow;
using namespace Rendering::Light;
using namespace Math;
using namespace Core;

DirectionalLight::DirectionalLight() : LightForm(), _projectionSize(0.0f), _useAutoProjectLocation(true)
{
	_type = LightType::Directional;
}

DirectionalLight::~DirectionalLight()
{
}

void DirectionalLight::CreateLightShadow(const std::function<void()>& addUpdateCounter)
{
	_shadow = new DirectionalLightShadow(this, addUpdateCounter);
}

void DirectionalLight::ComputeViewProjMatrix(const Intersection::BoundBox& sceneBoundBox)
{
	Matrix view;
	_owner->GetTransform()->FetchWorldMatrix(view);

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

	Matrix proj, invProj;
#if defined(USE_SHADOW_INVERTED_DEPTH)
	Matrix::OrthoLH(proj, orthogonalWH, orthogonalWH, DIRECTIONAL_LIGHT_FRUSTUM_MAX_Z, DIRECTIONAL_LIGHT_FRUSTUM_MIN_Z);
	Matrix::OrthoLH(invProj, orthogonalWH, orthogonalWH, DIRECTIONAL_LIGHT_FRUSTUM_MIN_Z, DIRECTIONAL_LIGHT_FRUSTUM_MAX_Z);
#else
	Matrix::OrthoLH(proj, orthogonalWH, orthogonalWH, DIRECTIONAL_LIGHT_FRUSTUM_MIN_Z, DIRECTIONAL_LIGHT_FRUSTUM_MAX_Z);
	Matrix::OrthoLH(invProj, orthogonalWH, orthogonalWH, DIRECTIONAL_LIGHT_FRUSTUM_MAX_Z, DIRECTIONAL_LIGHT_FRUSTUM_MIN_Z);
#endif

	_invViewProjMat = view * invProj;

	Matrix& viewProj = _viewProjMat;
	viewProj = view * proj;

#if defined(USE_SHADOW_INVERTED_DEPTH)
	_frustum.Make(_invViewProjMat);
#else
	_frustum.Make(viewProj);
#endif
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