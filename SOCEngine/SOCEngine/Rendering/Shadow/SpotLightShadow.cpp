#include "SpotLightShadow.h"
#include "SpotLight.h"
#include "Transform.h"
#include "Object.h"
#include "CameraForm.h"

using namespace Rendering::Shadow;
using namespace Rendering::Light;
using namespace Rendering::Camera;
using namespace Math;
using namespace Core;

SpotLightShadow::SpotLightShadow(const LightForm* owner) : ShadowCommon(owner)
{
}

SpotLightShadow::~SpotLightShadow()
{
}

void SpotLightShadow::MakeMatrixParam(Math::Matrix& outViewProjMat) const
{
	if(GetUseVSM())
		Math::Matrix::Transpose(outViewProjMat, _invNearFarViewProjMat);
	else
		Math::Matrix::Transpose(outViewProjMat, _viewProjMat);
}

void SpotLightShadow::ComputeViewProjMatrix()
{
	Matrix view;
	_owner->GetOwner()->GetTransform()->FetchWorldMatrix(view);
	CameraForm::GetViewMatrix(view, view);

	const SpotLight* owner	= static_cast<const SpotLight*>(_owner);
	float spotAngle			= Common::Deg2Rad(owner->GetSpotAngleDegree());

	float radius	= _owner->GetRadius();
	float projNear	= GetProjectionNear();

	Matrix proj, invNearFarProj;
	Matrix::PerspectiveFovLH(proj, 1.0f, spotAngle, radius, projNear);
	Matrix::PerspectiveFovLH(invNearFarProj, 1.0f, spotAngle, projNear, radius);

	_invNearFarViewProjMat	= view * invNearFarProj;
	_viewProjMat			= view * proj;
}