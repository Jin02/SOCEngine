#include "SpotLight.h"
#include "Object.h"
#include "MathCommon.h"
#include "CameraForm.h"

using namespace Intersection;
using namespace Rendering;
using namespace Rendering::Camera;
using namespace Rendering::Shadow;
using namespace Rendering::Light;
using namespace Math;
using namespace Core;

SpotLight::SpotLight()  : LightForm(),
	_spotAngleDegree(0)
{
	_type = LightType::Spot;
}

SpotLight::~SpotLight()
{

}

void SpotLight::CreateLightShadow(const std::function<void()>& addUpdateCounter)
{
	_shadow = new SpotLightShadow(this, addUpdateCounter);
}

void SpotLight::ComputeViewProjMatrix(const Intersection::BoundBox& sceneBoundBox)
{
	Matrix view;
	_owner->GetTransform()->FetchWorldMatrix(view);
	CameraForm::GetViewMatrix(view, view);

	Matrix proj, invProj;
#if defined(USE_SHADOW_INVERTED_DEPTH)
	Matrix::PerspectiveFovLH(proj, 1.0f, Common::Deg2Rad(_spotAngleDegree), _radius, _projNear);
	Matrix::PerspectiveFovLH(invProj, 1.0f, Common::Deg2Rad(_spotAngleDegree), _projNear, _radius);
#else
	Matrix::PerspectiveFovLH(proj, 1.0f, Common::Deg2Rad(_spotAngleDegree), _projNear, _radius);
	Matrix::PerspectiveFovLH(invProj, 1.0f, Common::Deg2Rad(_spotAngleDegree), _radius, _projNear);
#endif

	_invViewProjMat = view * invProj;

	Matrix& viewProj = _viewProjMat;
	viewProj = view * proj;
}

bool SpotLight::Intersect(const Intersection::Sphere &sphere) const
{
#if 0
	Vector3 wp;
	_owner->GetTransform()->FetchWorldPosition(wp);

	Cone cone(_spotAngleDegree, _radius, _owner->GetTransform()->GetForward(), wp);
	return cone.Intersects(sphere);
#else
	Core::Transform* tf = _owner->GetTransform();
	Math::Vector3 wp;
	tf->FetchWorldPosition(wp);

	return Sphere::Intersects(sphere, Sphere(wp, _radius));
#endif
}

void SpotLight::MakeLightBufferElement(LightTransformBuffer& outTransform, Params& outParam) const
{
	const Transform* transform = _owner->GetTransform();

	Transform worldTransform(nullptr);
	transform->FetchWorldTransform(worldTransform);
	outTransform.worldPosition = worldTransform.GetLocalPosition();

	const auto& forward = worldTransform.GetForward();
	outTransform.radius = forward.z >= 0.0f ? _radius : -_radius;

	outParam.dirX = Common::FloatToHalf(forward.x);
	outParam.dirY = Common::FloatToHalf(forward.y);

	float radian = Common::Deg2Rad(_spotAngleDegree);
	outParam.outerConeCosAngle = Math::Common::FloatToHalf(cos(radian));

	radian = Common::Deg2Rad(_spotAngleDegree * 0.3333f);
	outParam.innerConeCosAngle = Math::Common::FloatToHalf(cos(radian));
}

Component* SpotLight::Clone() const
{
	return new SpotLight(*this);
}

void SpotLight::SetSpotAngleDegree(float d)
{
	_spotAngleDegree = d;

	if(_owner)
		_owner->GetTransform()->AddUpdateCounter();
}