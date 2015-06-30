#include "DirectionalLight.h"
#include "Object.h"

using namespace Rendering;
using namespace Rendering::Light;


DirectionalLight::DirectionalLight() : LightForm()
{
	_type = LightType::Directional;
}

DirectionalLight::~DirectionalLight()
{
}

bool DirectionalLight::Intersects(const Intersection::Sphere &sphere)
{
	return true;
}

void DirectionalLight::MakeLightBufferElement(LightTransformBuffer& outTransform, Params& outParam) const
{
	const Core::Transform* transform = _owner->GetTransform();
	transform->FetchWorldPosition(outTransform.worldPosition);
	outTransform.radius = transform->GetForward().z;
	
	outParam.dirX = Math::Common::FloatToHalf(transform->GetForward().x);
	outParam.dirY = Math::Common::FloatToHalf(transform->GetForward().y);
}