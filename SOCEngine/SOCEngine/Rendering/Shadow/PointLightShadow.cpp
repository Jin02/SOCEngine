#include "PointLightShadow.h"
#include "PointLight.h"
#include "Transform.h"
#include "Object.hpp"

using namespace Rendering::Shadow;
using namespace Rendering::Light;
using namespace Math;
using namespace Core;

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
	float projNear	= _base.GetProjNear();

	Matrix proj = Matrix::PerspectiveFovLH(1.0f, DEG_2_RAD(90.0f), radius, projNear);

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

std::array<Math::Matrix, 6> PointLightShadow::MakeMatrixParam() const
{
	std::array<Math::Matrix, 6> viewProjMats = GetViewProjectionMatrices();

	for(uint i=0; i<6; ++i)
		viewProjMats[i] = Math::Matrix::Transpose(viewProjMats[i]);

	return viewProjMats;
}

std::array<Math::Matrix, 6> PointLightShadow::GetViewProjectionMatrices() const
{
	std::array<Math::Matrix, 6> viewProjMats;

	for(uint i=1; i<6; ++i)
		viewProjMats[i] = _viewProjMat[i-1];

	return viewProjMats;
}
