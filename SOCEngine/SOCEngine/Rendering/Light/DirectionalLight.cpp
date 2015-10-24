#include "DirectionalLight.h"
#include "Object.h"

using namespace Rendering;
using namespace Rendering::Light;
using namespace Math;
using namespace Core;

DirectionalLight::DirectionalLight() : LightForm()
{
	_type = LightType::Directional;
}

DirectionalLight::~DirectionalLight()
{
}

bool DirectionalLight::Intersect(const Intersection::Sphere &sphere) const
{
	return _frustum.In(sphere.center, sphere.radius);
}

void DirectionalLight::MakeLightBufferElement(LightTransformBuffer& outTransform, Params& outParam) const
{
	const Transform* transform = _owner->GetTransform();
	Transform worldTransform(nullptr);
	transform->FetchWorldTransform(worldTransform);
	outTransform.worldPosition = worldTransform.GetLocalPosition();

	const auto& forward = worldTransform.GetForward();
	outTransform.radius = forward.z;
	
	outParam.dirX = Math::Common::FloatToHalf(forward.x);
	outParam.dirY = Math::Common::FloatToHalf(forward.y);
}

Core::Component* DirectionalLight::Clone() const
{
	return new DirectionalLight(*this);
}