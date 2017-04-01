#include "SpotLight.h"
#include "Cone.h"
#include "GlobalDefine.h"

using namespace Intersection;
using namespace Rendering;
using namespace Rendering::Light;
using namespace Math;
using namespace Core;

SpotLight::SpotLight(ObjectId id)
	: _base(id)
{
}

bool SpotLight::Intersect(const Sphere &sphere, const Transform& transform) const
{
#if 0
	Cone cone(_spotAngleDegree, _base.GetRadius(), transform.GetWorldForward(), transform.GetWorldPosition());
	return cone.Intersects(sphere);
#else
	Math::Vector3 wp = transform.GetWorldPosition();
	return Sphere::Intersects(sphere, Sphere(wp, _base.GetRadius()));
#endif
}

void SpotLight::MakeTransform(TransformType& outTransform, const Transform& transform) const
{
	Vector3 forward = transform.GetWorldForward();
	float radius = (forward.z >= 0.0f) ? _base.GetRadius() : -_base.GetRadius();

	Vector3 wp = transform.GetWorldPosition();
	outTransform = Vector4(wp.x, wp.y, wp.z, radius);
}

void SpotLight::MakeParam(Param& outParam, const Core::Transform& transform)
{
	Vector3 forward = transform.GetWorldForward();

	outParam.dirX = Half(forward.x);
	outParam.dirY = Half(forward.y);

	outParam.outerConeCosAngle = Half(cos(DEG_2_RAD(_spotAngleDegree)));
	outParam.innerConeCosAngle = Half(cos(DEG_2_RAD(_spotAngleDegree * 0.3333f)));
}