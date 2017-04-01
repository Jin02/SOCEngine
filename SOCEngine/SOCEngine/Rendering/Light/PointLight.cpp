#include "PointLight.h"
#include <assert.h>

using namespace Intersection;
using namespace Rendering;
using namespace Rendering::Light;
using namespace Math;
using namespace Core;

PointLight::PointLight(Core::ObjectId id)
	:_base(id)
{
}

bool PointLight::Intersect(const Sphere &sphere, const Transform& transform) const
{
	assert(transform.GetObjectId() == _base.GetObjectId());
	Vector3 wp = transform.GetWorldPosition();

	return Sphere::Intersects(sphere, Sphere(wp, _base.GetRadius()));
}

void PointLight::MakeTransform(TransformType& outTransform, const Transform& transform) const
{
	assert(transform.GetObjectId() == _base.GetObjectId());
	Vector3 wp = transform.GetWorldPosition();

	outTransform = Vector4(wp.x, wp.y, wp.z, _base.GetRadius());
}