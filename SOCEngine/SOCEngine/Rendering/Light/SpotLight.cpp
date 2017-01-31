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

void SpotLight::CreateShadow()
{
	_shadow = new SpotLightShadow(this);
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

void SpotLight::MakeTransform(LightTransformBuffer& outTransform) const
{
	const Transform* transform	= _owner->GetTransform();
	const Matrix& worldMat		= transform->GetWorldMatrix();
	
	outTransform.worldPosition	= Vector4(worldMat._41, worldMat._42, worldMat._43);
	outTransform.radius		= worldMat._33 >= 0.0f ? _radius : -_radius;
}

void SpotLight::MakeParam(Param& outParam) const
{
	const Transform* transform	= _owner->GetTransform();
	const Matrix& worldMat		= transform->GetWorldMatrix();
	
	outParam.dirXY = LightForm::DirXYHalf(worldMat._13, worldMat._23);
	
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
