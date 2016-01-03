#include "PointLightShadow.h"
#include "PointLight.h"

using namespace Rendering::Shadow;
using namespace Rendering::Light;

PointLightShadow::PointLightShadow(const LightForm* owner,
								   const std::function<void()>& ownerUpdateCounter)
	: ShadowCommon(owner, ownerUpdateCounter)
{
}

PointLightShadow::~PointLightShadow()
{
}

void PointLightShadow::MakeParam(Param& outParam, bool useVSM) const
{
	ShadowCommon::MakeParam(outParam);

	const PointLight* light = dynamic_cast<const PointLight*>(_owner);

#ifdef USE_SHADOW_INVERTED_DEPTH
	if(useVSM)
		light->GetInvViewProjMatrices(outParam.viewProjMat);
	else
#endif
	light->GetViewProjectionMatrices(outParam.viewProjMat);

	for(uint i=0; i<6; ++i)
		Math::Matrix::Transpose(outParam.viewProjMat[i], outParam.viewProjMat[i]);
}