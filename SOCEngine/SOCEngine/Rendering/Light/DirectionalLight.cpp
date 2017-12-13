#include "DirectionalLight.h"
#include "Object.h"
#include <assert.h>
#include "LightManager.h"

using namespace Rendering;
using namespace Rendering::Light;
using namespace Math;
using namespace Core;

void DirectionalLight::UpdateFlag(const Transform& transform)
{
	assert(transform.GetObjectID() == _base.GetObjectID());

	bool isNegDirZSign = transform.GetWorldMatrix()._33 < 0.0f;
	_base.SetFlag( (_base.GetFlag() & 0xfe) | (isNegDirZSign ? 1 : 0) );
}

DirectionalLight::TransformType DirectionalLight::MakeTransform(const Transform& transform) const
{
	assert(transform.GetObjectID() == _base.GetObjectID());

	const auto& forward = transform.GetLocalForward();
	return TransformType(Half(forward.x), Half(forward.y));
}
