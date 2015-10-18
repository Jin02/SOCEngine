#include "PointLight.h"
#include "Object.h"

using namespace Intersection;
using namespace Rendering;
using namespace Rendering::Light;


PointLight::PointLight() : LightForm()
{
	_type = LightType::Point;
}

PointLight::~PointLight(void)
{
}

bool PointLight::Intersect(const Sphere &sphere) const
{
	Core::Transform* tf = _owner->GetTransform();
	Math::Vector3 wp;
	tf->FetchWorldPosition(wp);

	return Sphere::Intersects(sphere, Sphere(wp, _radius));
}

void PointLight::MakeLightBufferElement(LightTransformBuffer& out) const
{
	_owner->GetTransform()->FetchWorldPosition(out.worldPosition);
	out.radius = _radius;
}

Core::Component* PointLight::Clone() const
{
	return new PointLight(*this);
}