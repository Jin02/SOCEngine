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

void SpotLightShadow::MakeParam(Param& outParam, bool useVSM) const
{
	ShadowCommon::MakeParam(outParam);

#ifdef USE_SHADOW_INVERTED_DEPTH
	if(useVSM)
		outParam.viewProjMat = _owner->GetInvViewProjectionMatrix();
	else
#endif
	outParam.viewProjMat = _owner->GetViewProjectionMatrix();

	Math::Matrix::Transpose(outParam.viewProjMat, outParam.viewProjMat);
}