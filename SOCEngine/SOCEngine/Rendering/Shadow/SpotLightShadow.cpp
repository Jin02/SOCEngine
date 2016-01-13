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

void SpotLightShadow::MakeMatrixParam(Math::Matrix& outViewProjMat, Math::Matrix& outInvVPVMat) const
{
#ifdef USE_SHADOW_INVERTED_DEPTH
	if(GetUseVSM())
		Math::Matrix::Transpose(outViewProjMat, _invNearFarViewProjMat);
	else
#endif
	Math::Matrix::Transpose(outViewProjMat, _viewProjMat);
	Math::Matrix::Transpose(outInvVPVMat, _invViewProjViewportMat);
}

void SpotLightShadow::ComputeViewProjMatrix(const Math::Matrix& invViewportMat)
{
	Matrix view;
	_owner->GetOwner()->GetTransform()->FetchWorldMatrix(view);
	CameraForm::GetViewMatrix(view, view);

	const SpotLight* owner	= static_cast<const SpotLight*>(_owner);
	float spotAngle			= Common::Deg2Rad(owner->GetSpotAngleDegree());

	float radius	= _owner->GetRadius();
	float projNear	= GetProjectionNear();

	Matrix proj, invNearFarProj;
#if defined(USE_SHADOW_INVERTED_DEPTH)
	Matrix::PerspectiveFovLH(proj, 1.0f, spotAngle, radius, projNear);
	Matrix::PerspectiveFovLH(invNearFarProj, 1.0f, spotAngle, projNear, radius);
#else
	Matrix::PerspectiveFovLH(proj, 1.0f, spotAngle, projNear, radius);
	Matrix::PerspectiveFovLH(invNearFarProj, 1.0f, spotAngle, radius, projNear);
#endif

	_invNearFarViewProjMat	= view * invNearFarProj;
	_viewProjMat			= view * proj;
	
	Matrix invViewProj;
	Matrix::Inverse(invViewProj, _viewProjMat);

	_invViewProjViewportMat = invViewportMat * invViewProj;
}