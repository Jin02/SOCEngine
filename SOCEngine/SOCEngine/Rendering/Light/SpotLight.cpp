#include "SpotLight.h"
#include "Object.h"
#include "MathCommon.h"

using namespace Intersection;
using namespace Rendering;
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

bool SpotLight::Intersects(const Intersection::Sphere &sphere)
{
	Vector3 wp;
	_owner->GetTransform()->FetchWorldPosition(wp);

	Cone cone(_spotAngleDegree, _radius, _owner->GetTransform()->GetForward(), wp);
	return cone.Intersects(sphere);
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