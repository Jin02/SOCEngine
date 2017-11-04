#include "PointLight.h"
#include <assert.h>
#include "LightManager.h"

using namespace Intersection;
using namespace Rendering;
using namespace Rendering::Light;
using namespace Math;
using namespace Core;

bool PointLight::Intersect(const Sphere& sphere, const TransformPool& tfPool) const
{
	const auto* transform = tfPool.Find(GetObjectID().Literal()); assert(transform);
	Vector3 wp = transform->GetWorldPosition();

	return Sphere::Intersects(sphere, Sphere(wp, _base.GetRadius()));
}

bool PointLight::Intersect(const Frustum& frustum, const TransformPool& tfPool) const
{
	const auto* transform = tfPool.Find(GetObjectID().Literal()); assert(transform);
	return frustum.In(transform->GetWorldPosition(), _base.GetRadius());
}

PointLight::TransformType PointLight::MakeTransform(const Transform& transform) const
{
	assert(transform.GetObjectID() == _base.GetObjectID());
	const Vector3& wp = transform.GetWorldPosition();

	return Vector4(wp.x, wp.y, wp.z, _base.GetRadius());
}
