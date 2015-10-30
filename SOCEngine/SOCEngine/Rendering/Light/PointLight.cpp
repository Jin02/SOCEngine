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

	Matrix proj;
	Matrix::PerspectiveFovLH(proj, 1.0f, Common::Deg2Rad(90.0f), _radius, 1.0f);

	auto ComputeCameraConstBufferData = [](Matrix& outView, Matrix& viewProj,
		const Vector3& eyePos, const Vector3& forward, const Vector3& up, const Matrix& projMat)
	{
		Matrix& view = outView;
		{
			Transform tf0(nullptr);
			tf0.UpdatePosition(eyePos);
			tf0.LookAtWorld(eyePos + forward, &up);

			tf0.FetchWorldMatrix(view);
			CameraForm::GetViewMatrix(view, view);
		}

		viewProj = view * projMat;
	};

	Vector3 worldPos;
	_owner->GetTransform()->FetchWorldPosition(worldPos);

	Matrix view, viewProj;
	ComputeCameraConstBufferData(view, viewProj, worldPos, forwards[0], ups[0], proj);
	bool isDifferent = memcmp(&_prevViewProj, &viewProj, sizeof(Matrix)) != 0;
	if(isDifferent)
	{
		LightForm::_viewMat = view;
		LightForm::_viewProjMat = viewProj;

		for(uint i=1; i<6; ++i)
		{
			uint matIdx = i - 1;
			ComputeCameraConstBufferData(_viewMat[matIdx], _viewProjMat[matIdx], worldPos, forwards[i], ups[i], proj);
		}
	}
}

void PointLight::GetViewMatrices(std::array<Math::Matrix, 6>& out) const
{
	out[0] = LightForm::_viewMat;

	for(uint i=1; i<6; ++i)
		out[i] = _viewMat[i-1];
}

void PointLight::GetViewProjectionMatrices(std::array<Math::Matrix, 6>& out) const
{
	out[0] = LightForm::_viewProjMat;

	for(uint i=1; i<6; ++i)
		out[i] = _viewProjMat[i-1];
}