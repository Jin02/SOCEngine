#include "DirectionalLight.h"
#include "Object.h"
#include "CameraForm.h"

using namespace Rendering;
using namespace Rendering::Camera;
using namespace Rendering::Shadow;
using namespace Rendering::Light;
using namespace Math;
using namespace Core;

DirectionalLight::Param::Param(float dirX, float dirY)
{
	this->dirX	= Math::Common::FloatToHalf(dirX);
	this->dirY	= Math::Common::FloatToHalf(dirY);
}

DirectionalLight::DirectionalLight() : LightForm()
{
	_type = LightType::Directional;
}

DirectionalLight::~DirectionalLight()
{
}

void DirectionalLight::CreateShadow()
{
	_shadow = new DirectionalLightShadow(this);
}

void DirectionalLight::OnUpdate(float deltaTime)
{
	bool isNegDirZSign = _owner->GetTransform()->GetWorldMatrix()._33 < 0.0f;
	_flag = (_flag & 0xfe) | (isNegDirZSign ? 1 : 0);
}

bool DirectionalLight::Intersect(const Intersection::Sphere &sphere) const
{
	return true;
}

void DirectionalLight::MakeParam(DirXYHalf& outDirXY) const
{
	const Transform* transform = _owner->GetTransform();

	Transform worldTransform(nullptr);
	transform->FetchWorldTransform(worldTransform);
	const auto& forward = worldTransform.GetForward();
	
	outParam = DirXYHalf(forward.x, forward.y);
}

Core::Component* DirectionalLight::Clone() const
{
	return new DirectionalLight(*this);
}
