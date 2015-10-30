#include "DirectionalLightShadow.h"
#include "DirectionalLight.h"

using namespace Rendering::Shadow;
using namespace Rendering::Light;

DirectionalLightShadow::DirectionalLightShadow(const LightForm* owner,
											   const std::function<void()>& ownerUpdateCounter)
	: ShadowCommon(owner, ownerUpdateCounter)
{
}

DirectionalLightShadow::~DirectionalLightShadow()
{
}

void DirectionalLightShadow::MakeParam(Param& outParam) const
{
	ShadowCommon::MakeParam(outParam);

	outParam.viewProjMat = _owner->GetViewProjectionMatrix();
}