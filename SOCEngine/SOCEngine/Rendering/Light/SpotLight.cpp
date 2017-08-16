#include "SpotLight.h"
#include "Cone.h"
#include "GlobalDefine.h"
#include <assert.h>
#include "LightManager.h"

using namespace Intersection;
using namespace Rendering;
using namespace Rendering::Light;
using namespace Math;
using namespace Core;

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

SpotLight::TransformType SpotLight::MakeTransform(const Transform& transform) const
{
	assert(transform.GetObjectID() == _base.GetObjectID());
	Vector3 forward = transform.GetWorldForward();
	float radius = (forward.z >= 0.0f) ? _base.GetRadius() : -_base.GetRadius();

	Vector3 wp = transform.GetWorldPosition();
	return Vector4(wp.x, wp.y, wp.z, radius);
}

SpotLight::Param SpotLight::MakeParam(const Core::Transform& transform) const
{
	assert(transform.GetObjectID() == _base.GetObjectID());
	Vector3 forward = transform.GetWorldForward();

	Param param;
	{
		param.dirX = Half(forward.x);
		param.dirY = Half(forward.y);
		param.outerConeCosAngle = Half(cos(DEG_2_RAD(_spotAngleDegree)));
		param.innerConeCosAngle = Half(cos(DEG_2_RAD(_spotAngleDegree * 0.3333f)));
	}
	
	return param;
}
