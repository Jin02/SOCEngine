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
	assert(transform.GetObjectId() == _base.GetObjectId());

	bool isNegDirZSign = transform.GetWorldMatrix()._33 < 0.0f;
	_base.SetFlag( (_base.GetFlag() & 0xfe) | (isNegDirZSign ? 1 : 0) );
}

DirectionalLight::TransformType DirectionalLight::MakeTransform(const Transform& transform) const
{
	assert(transform.GetObjectId() == _base.GetObjectId());

	const auto& forward = transform.GetForward();
	return TransformType(Half(forward.x), Half(forward.y));
}
