#include "PointLight.h"
#include "Object.h"
#include "CameraForm.h"

using namespace Intersection;
using namespace Rendering;
using namespace Rendering::Camera;
using namespace Rendering::Shadow;
using namespace Rendering::Light;
using namespace Math;
using namespace Core;

PointLight::PointLight() : LightForm()
{
	_type = LightType::Point;
}

PointLight::~PointLight(void)
{
}

void PointLight::CreateShadow()
{
	_shadow = new PointLightShadow(this);
}

bool PointLight::Intersect(const Sphere &sphere) const
{
	Core::Transform* tf = _owner->GetTransform();
	Math::Vector3 wp;
	tf->FetchWorldPosition(wp);

	return Sphere::Intersects(sphere, Sphere(wp, _radius));
}

void PointLight::MakeTransform(TransformType& out) const
{
	_owner->GetTransform()->FetchWorldPosition(out.worldPosition);
	out.radius = _radius;
}

Core::Component* PointLight::Clone() const
{
	return new PointLight(*this);
}
