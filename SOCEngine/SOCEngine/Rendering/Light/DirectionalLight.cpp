#include "DirectionalLight.h"
#include "Object.h"
#include "CameraForm.h"

using namespace Rendering;
using namespace Rendering::Camera;
using namespace Rendering::Shadow;
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

void DirectionalLight::MakeTransform(TransformType& outDirXY) const
{
	const Transform* transform	= _owner->GetTransform();
	const Matrix& worldMat		= transform->GetWorldMatrix();
	const Vector3 forward		= Vector3(worldMat._13, worldMat._23, worldMat._33);
	
	outDirXY = TransformType(forward.x, forward.y);
}

Core::Component* DirectionalLight::Clone() const
{
	return new DirectionalLight(*this);
}
