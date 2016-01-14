#include "PointLightShadow.h"
#include "PointLight.h"
#include "Transform.h"
#include "CameraForm.h"
#include "Object.h"

using namespace Rendering::Shadow;
using namespace Rendering::Light;
using namespace Rendering::Camera;
using namespace Math;
using namespace Core;

PointLightShadow::PointLightShadow(const LightForm* owner)
	: ShadowCommon(owner), _underScanSize(4.25f)
{
}

PointLightShadow::~PointLightShadow()
{
}

void PointLightShadow::MakeParam(Param& outParam, uint lightIndex) const
{
	ShadowCommon::MakeParam(outParam, lightIndex);
	outParam.underScanSize = _underScanSize;
}

void PointLightShadow::ComputeViewProjMatrix(const Math::Matrix& invViewportMat)
{
	Vector3 forwards[6] = 
	{
		Vector3( 0.0f,  0.0f,  1.0f),
		Vector3( 0.0f,  0.0f, -1.0f),
		Vector3( 1.0f,  0.0f,  0.0f),
		Vector3(-1.0f,  0.0f,  0.0f),
		Vector3( 0.0f,  1.0f,  0.0f),
		Vector3( 0.0f, -1.0f,  0.0f)
	};
	Vector3 ups[6] = 
	{
		Vector3( 0.0f,  1.0f,  0.0f),
		Vector3( 0.0f,  1.0f,  0.0f),
		Vector3( 0.0f,  1.0f,  0.0f),
		Vector3( 0.0f,  1.0f,  0.0f),
		Vector3( 0.0f,  0.0f, -1.0f),
		Vector3( 0.0f,  0.0f,  1.0f),
	};

	float radius	= _owner->GetRadius();
	float projNear	= GetProjectionNear();

	Matrix proj, invNearFarProj;
#if defined(USE_SHADOW_INVERTED_DEPTH)
	Matrix::PerspectiveFovLH(proj, 1.0f, Common::Deg2Rad(90.0f), radius, projNear);
	Matrix::PerspectiveFovLH(invNearFarProj, 1.0f, Common::Deg2Rad(90.0f), projNear, radius);
#else
	Matrix::PerspectiveFovLH(proj, 1.0f, Common::Deg2Rad(90.0f), projNear, radius);
	Matrix::PerspectiveFovLH(invNearFarProj, 1.0f, Common::Deg2Rad(90.0f), radius, projNear);
#endif

	auto ComputeViewProj = [](Matrix& outInvNearFarViewProj, Matrix& outViewProj,
		const Vector3& eyePos, const Vector3& forward, const Vector3& up, const Matrix& projMat, const Matrix& invNearFarProjMat)
	{
		Matrix view;
		{
			Transform tf0(nullptr);
			tf0.UpdatePosition(eyePos);
			tf0.LookAtWorld(eyePos + forward, &up);

			tf0.FetchWorldMatrix(view);
			CameraForm::GetViewMatrix(view, view);
		}

		outViewProj				= view * projMat;
		outInvNearFarViewProj	= view * invNearFarProjMat;
	};

	Vector3 worldPos;
	_owner->GetOwner()->GetTransform()->FetchWorldPosition(worldPos);

	Matrix invNearFarViewProj, viewProj;
	ComputeViewProj(invNearFarViewProj, viewProj, worldPos, forwards[0], ups[0], proj, invNearFarProj);
	bool isDifferent = memcmp(&_prevViewProj, &viewProj, sizeof(Matrix)) != 0;
	if(isDifferent)
	{
		_prevViewProj = viewProj;

		ShadowCommon::_invNearFarViewProjMat	= invNearFarViewProj;
		ShadowCommon::_viewProjMat				= viewProj;

		// Compute InvViewProjViewportMat
		{
			Matrix invViewProj;
			Matrix::Inverse(invViewProj, viewProj);
			ShadowCommon::_invViewProjViewportMat = invViewportMat * invViewProj;
		}

		for(uint i=1; i<6; ++i)
		{
			uint matIdx = i - 1;
			
			ComputeViewProj(_invNearFarViewProjMat[matIdx], _viewProjMat[matIdx],
							worldPos, forwards[i], ups[i], proj, invNearFarProj);

			// Compute InvViewProjViewportMat
			{
				Matrix invNearFarViewProj;
				Matrix::Inverse(invNearFarViewProj, _viewProjMat[matIdx]);

				_invViewProjViewportMat[matIdx] = invViewportMat * invNearFarViewProj;
			}
		}
	}
}

void PointLightShadow::MakeMatrixParam(std::array<Math::Matrix, 6>& outViewProjMat, std::array<Math::Matrix, 6>& outInvVPVMat) const
{
#ifdef USE_SHADOW_INVERTED_DEPTH
	if(GetUseVSM())
		GetInvNearFarViewProjMatrices(outViewProjMat);
	else
#endif
	GetViewProjectionMatrices(outViewProjMat);
	GetInvViewProjViewportMatrices(outInvVPVMat);

	for(uint i=0; i<6; ++i)
	{
		Math::Matrix::Transpose(outViewProjMat[i], outViewProjMat[i]);
		Math::Matrix::Transpose(outInvVPVMat[i], outInvVPVMat[i]);
	}
}

void PointLightShadow::GetInvNearFarViewProjMatrices(std::array<Math::Matrix, 6>& out) const
{
	out[0] = ShadowCommon::_invNearFarViewProjMat;

	for(uint i=1; i<6; ++i)
		out[i] = _invNearFarViewProjMat[i-1];
}

void PointLightShadow::GetViewProjectionMatrices(std::array<Math::Matrix, 6>& out) const
{
	out[0] = ShadowCommon::_viewProjMat;

	for(uint i=1; i<6; ++i)
		out[i] = _viewProjMat[i-1];
}

void PointLightShadow::GetInvViewProjViewportMatrices(std::array<Math::Matrix, 6>& out) const
{
	out[0] = ShadowCommon::_invViewProjViewportMat;

	for(uint i=1; i<6; ++i)
		out[i] = _invViewProjViewportMat[i-1];
}