#include "DirectionalLightShadow.h"
#include "DirectionalLight.h"

using namespace Rendering::Shadow;
using namespace Rendering::Light;


void DirectionalLightShadow::MakeMatrixParam(Math::Matrix& outViewProjMat) const
{
	const DirectionalLight* owner = dynamic_cast<const DirectionalLight*>(_owner);

	Math::Matrix::Transpose(outViewProjMat,		owner->GetViewProjectionMatrix());
}