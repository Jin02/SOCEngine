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

void DirectionalLightShadow::MakeParam(Param& outParam, bool useVSM) const
{
	ShadowCommon::MakeParam(outParam);

#ifdef USE_SHADOW_INVERTED_DEPTH
	if(useVSM)
		outParam.viewProjMat = _owner->GetInvViewProjectionMatrix();
	else
#else
		outParam.viewProjMat = _owner->GetViewProjectionMatrix();
#endif

	Math::Matrix::Transpose(outParam.viewProjMat, outParam.viewProjMat);
}