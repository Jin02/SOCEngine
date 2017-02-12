#include "DirectionalLightShadow.h"
#include "DirectionalLight.h"

using namespace Rendering::Shadow;
using namespace Rendering::Light;

DirectionalLightShadow::DirectionalLightShadow(const LightForm* owner)
	: ShadowCommon(owner)
{
	SetBias(0.0f);
}

DirectionalLightShadow::~DirectionalLightShadow()
{
}

void DirectionalLightShadow::MakeMatrixParam(Math::Matrix& outViewProjMat) const
{
	const DirectionalLight* owner = dynamic_cast<const DirectionalLight*>(_owner);

	Math::Matrix::Transpose(outViewProjMat,		owner->GetViewProjectionMatrix());
}