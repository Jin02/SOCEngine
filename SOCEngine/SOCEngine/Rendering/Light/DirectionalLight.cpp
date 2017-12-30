#include "DirectionalLight.h"
#include "Object.h"
#include <assert.h>
#include "LightManager.h"

using namespace Rendering;
using namespace Rendering::Light;
using namespace Math;
using namespace Core;

DirectionalLight::TransformType DirectionalLight::MakeTransform(const Transform& transform) const
{
	assert(transform.GetObjectID() == _base.GetObjectID());

	bool isNegDirZSign = transform.GetWorldMatrix()._33 < 0.0f;
	const_cast<BaseLight&>(_base).SetFlag( (_base.GetFlag() & 0xfe) | (isNegDirZSign ? 1 : 0) );

	const auto& forward = transform.GetLocalForward();
	return TransformType(Half(forward.x), Half(forward.y));
}
