#include "PointLight.h"
#include <assert.h>
#include "LightManager.h"

using namespace Intersection;
using namespace Rendering;
using namespace Rendering::Light;
using namespace Math;
using namespace Core;

bool PointLight::Intersect(const Sphere &sphere, const Transform& transform) const
{
	assert(transform.GetObjectId() == _base.GetObjectId());
	Vector3 wp = transform.GetWorldPosition();

	return Sphere::Intersects(sphere, Sphere(wp, _base.GetRadius()));
}

PointLight::TransformType PointLight::MakeTransform(const Transform& transform) const
{
	assert(transform.GetObjectId() == _base.GetObjectId());
	Vector3 wp = transform.GetWorldPosition();

	return Vector4(wp.x, wp.y, wp.z, _base.GetRadius());
}
