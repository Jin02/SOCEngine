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

void PointLightShadow::MakeParam(Param& outParam) const
{
	ShadowCommon::MakeParam(outParam);

	const PointLight* light = dynamic_cast<const PointLight*>(_owner);
	light->GetViewProjectionMatrices(outParam.viewProjMat);
}