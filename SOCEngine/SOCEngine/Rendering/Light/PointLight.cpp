#include "PointLight.h"
#include "Object.h"
#include "CameraForm.h"

using namespace Intersection;
using namespace Rendering;
using namespace Rendering::Camera;
using namespace Rendering::Shadow;
using namespace Rendering::Light;
using namespace Math;
using namespace Core;

PointLight::PointLight() : LightForm()
{
	_type = LightType::Point;
}

PointLight::~PointLight(void)
{
}

void PointLight::CreateLightShadow(const std::function<void()>& addUpdateCounter)
{
	_shadow = new PointLightShadow(this, addUpdateCounter);
}

bool PointLight::Intersect(const Sphere &sphere) const
{
	Core::Transform* tf = _owner->GetTransform();
	Math::Vector3 wp;
	tf->FetchWorldPosition(wp);

	return Sphere::Intersects(sphere, Sphere(wp, _radius));
}

void PointLight::MakeLightBufferElement(LightTransformBuffer& out) const
{
	_owner->GetTransform()->FetchWorldPosition(out.worldPosition);
	out.radius = _radius;
}

Core::Component* PointLight::Clone() const
{
	return new PointLight(*this);
}

void PointLight::ComputeViewProjMatrix(const Intersection::BoundBox& sceneBoundBox)
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

	Matrix proj, invProj;
#if defined(USE_SHADOW_INVERTED_DEPTH)
	Matrix::PerspectiveFovLH(proj, 1.0f, Common::Deg2Rad(90.0f), _radius, _projNear);
	Matrix::PerspectiveFovLH(invProj, 1.0f, Common::Deg2Rad(90.0f), _projNear, _radius);
#else
	Matrix::PerspectiveFovLH(proj, 1.0f, Common::Deg2Rad(90.0f), _projNear, _radius);
	Matrix::PerspectiveFovLH(invProj, 1.0f, Common::Deg2Rad(90.0f), _radius, _projNear);
#endif

	auto ComputeViewProj = [](Matrix& outInvViewProj, Matrix& outViewProj,
		const Vector3& eyePos, const Vector3& forward, const Vector3& up, const Matrix& projMat, const Matrix& invProjMat)
	{
		Matrix view;
		{
			Transform tf0(nullptr);
			tf0.UpdatePosition(eyePos);
			tf0.LookAtWorld(eyePos + forward, &up);

			tf0.FetchWorldMatrix(view);
			CameraForm::GetViewMatrix(view, view);
		}

		outViewProj		= view * projMat;
		outInvViewProj	= view * invProjMat;
	};

	Vector3 worldPos;
	_owner->GetTransform()->FetchWorldPosition(worldPos);

	Matrix invViewProj, viewProj;
	ComputeViewProj(invViewProj, viewProj, worldPos, forwards[0], ups[0], proj, invProj);
	bool isDifferent = memcmp(&_prevViewProj, &viewProj, sizeof(Matrix)) != 0;
	if(isDifferent)
	{
		_prevViewProj = viewProj;

		LightForm::_invViewProjMat	= invViewProj;
		LightForm::_viewProjMat		= viewProj;

		for(uint i=1; i<6; ++i)
		{
			uint matIdx = i - 1;
			ComputeViewProj(_invViewProjMatOffsetOne[matIdx], _viewProjMatOffsetOne[matIdx], worldPos, forwards[i], ups[i], proj, invProj);
		}
	}
}

void PointLight::GetInvViewProjMatrices(std::array<Math::Matrix, 6>& out) const
{
	out[0] = LightForm::_invViewProjMat;

	for(uint i=1; i<6; ++i)
		out[i] = _invViewProjMatOffsetOne[i-1];
}

void PointLight::GetViewProjectionMatrices(std::array<Math::Matrix, 6>& out) const
{
	out[0] = LightForm::_viewProjMat;

	for(uint i=1; i<6; ++i)
		out[i] = _viewProjMatOffsetOne[i-1];
}