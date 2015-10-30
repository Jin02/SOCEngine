#include "SpotLightShadow.h"
#include "SpotLight.h"

using namespace Rendering::Shadow;
using namespace Rendering::Light;

SpotLightShadow::SpotLightShadow(const LightForm* owner, const std::function<void()>& ownerUpdateCounter)
	: ShadowCommon(owner, ownerUpdateCounter)
{
}

SpotLightShadow::~SpotLightShadow()
{
}

void SpotLightShadow::MakeParam(Param& outParam) const
{
	ShadowCommon::MakeParam(outParam);

	outParam.viewProjMat = _owner->GetViewProjectionMatrix();
}