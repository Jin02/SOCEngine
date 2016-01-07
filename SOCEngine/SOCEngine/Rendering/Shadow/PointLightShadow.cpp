#include "PointLightShadow.h"
#include "PointLight.h"

using namespace Rendering::Shadow;
using namespace Rendering::Light;

PointLightShadow::PointLightShadow(const LightForm* owner,
								   const std::function<void()>& ownerUpdateCounter)
	: ShadowCommon(owner, ownerUpdateCounter),
	_underScanSize(4.25f)
{
}

PointLightShadow::~PointLightShadow()
{
}

void PointLightShadow::MakeParam(Param& outParam, bool useVSM) const
{
	const PointLight* light = dynamic_cast<const PointLight*>(_owner);

#ifdef USE_SHADOW_INVERTED_DEPTH
	if(useVSM)
		light->GetInvViewProjMatrices(outParam.viewProjMat);
	else
#endif
	light->GetViewProjectionMatrices(outParam.viewProjMat);

	for(uint i=0; i<6; ++i)
		Math::Matrix::Transpose(outParam.viewProjMat[i], outParam.viewProjMat[i]);

	uint packedIdx	= (FetchShadowCastingLightIndex() + 1) << 16;
	uint packedBias	= uint(GetBias() * 65536.0f);

	outParam.indexWithBias = packedIdx | packedBias;
	outParam.underScanSize = _underScanSize;
}