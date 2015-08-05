#include "SpotLight.h"
#include "Object.h"
#include "MathCommon.h"

using namespace Intersection;
using namespace Rendering;
using namespace Rendering::Light;

SpotLight::SpotLight()  : LightForm(),
	_spotAngleDegree(0), _falloff(0)
{
	_type = LightType::Spot;
}

SpotLight::~SpotLight()
{

}

bool SpotLight::Intersects(const Intersection::Sphere &sphere)
{
	Math::Vector3 wp;
	_owner->GetTransform()->FetchWorldPosition(wp);

	float degree = Math::Common::Rad2Deg(_param.coneCosAngle);
	Cone cone(degree, _radius, _owner->GetTransform()->GetForward(), wp);

	return cone.Intersects(sphere);
}

void SpotLight::MakeLightBufferElement(LightTransformBuffer& outTransform, Params& outParam) const
{
	const Core::Transform* transform = _owner->GetTransform();
	transform->FetchWorldPosition(outTransform.worldPosition);
	outTransform.radius = _radius;
	
	outParam.dirX = Math::Common::FloatToHalf(transform->GetForward().x);
	outParam.dirY = Math::Common::FloatToHalf(transform->GetForward().y);

	float radian = Math::Common::Deg2Rad(_spotAngleDegree);

	outParam.coneCosAngle			= Math::Common::FloatToHalf(radian);
	outParam.falloffWithDirZSignBit = Math::Common::FloatToHalf( _falloff * (1.0f - 2.0f * (transform->GetForward().z < 0)) );
}

Core::Component* SpotLight::Clone() const
{
	return new SpotLight(*this);
}