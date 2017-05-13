#include "PointLightShadow.h"
#include "PointLight.h"
#include "Transform.h"
#include "CameraForm.h"
#include "Object.hpp"

using namespace Rendering::Shadow;
using namespace Rendering::Light;
using namespace Rendering::Camera;
using namespace Math;
using namespace Core;

PointLightShadow::PointLightShadow(const LightForm* owner)
	: ShadowCommon(owner)
{
}

PointLightShadow::~PointLightShadow()
{
}

void PointLightShadow::ComputeViewProjMatrix()
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

	Matrix proj;
	Matrix::PerspectiveFovLH(proj, 1.0f, Common::Deg2Rad(90.0f), radius, projNear);

	auto ComputeViewProj = [](Matrix& outViewProj,
		const Vector3& eyePos, const Vector3& forward, const Vector3& up, const Matrix& projMat)
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
	};

	Vector3 worldPos;
	_owner->GetOwner()->GetTransform()->FetchWorldPosition(worldPos);

	Matrix viewProj;
	ComputeViewProj(viewProj, worldPos, forwards[0], ups[0], proj);
	bool isDifferent = memcmp(&_prevViewProj, &viewProj, sizeof(Matrix)) != 0;
	if(isDifferent)
	{
		_prevViewProj = viewProj;

		ShadowCommon::_viewProjMat				= viewProj;

		for(uint i=1; i<6; ++i)
		{
			uint matIdx = i - 1;
			
			ComputeViewProj(_viewProjMat[matIdx],
							worldPos, forwards[i], ups[i], proj);
		}
	}
}

void PointLightShadow::MakeMatrixParam(std::array<Math::Matrix, 6>& outViewProjMat) const
{
	GetViewProjectionMatrices(outViewProjMat);

	for(uint i=0; i<6; ++i)
	{
		Math::Matrix::Transpose(outViewProjMat[i], outViewProjMat[i]);
	}
}

void PointLightShadow::GetViewProjectionMatrices(std::array<Math::Matrix, 6>& out) const
{
	out[0] = ShadowCommon::_viewProjMat;

	for(uint i=1; i<6; ++i)
		out[i] = _viewProjMat[i-1];
}
