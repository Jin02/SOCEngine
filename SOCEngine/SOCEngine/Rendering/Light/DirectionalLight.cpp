#include "DirectionalLight.h"
#include "Object.h"
#include "CameraForm.h"

using namespace Rendering;
using namespace Rendering::Camera;
using namespace Rendering::Shadow;
using namespace Rendering::Light;
using namespace Math;
using namespace Core;

DirectionalLight::DirectionalLight() : LightForm(), _projectionSize(0.0f), _useAutoProjectLocation(true)
{
	_type = LightType::Directional;
}

DirectionalLight::~DirectionalLight()
{
}

void DirectionalLight::CreateShadow()
{
	_shadow = new DirectionalLightShadow(this);
}

void DirectionalLight::OnUpdate(float deltaTime)
{
	bool isNegDirZSign = _owner->GetTransform()->GetWorldMatrix()._33 < 0.0f;

	_flag |= isNegDirZSign ? 1 : 0;
}

void DirectionalLight::ComputeViewProjMatrix(const Intersection::BoundBox& sceneBoundBox, const Math::Matrix& invViewportMat)
{
	Matrix view;
	_owner->GetTransform()->FetchWorldMatrix(view);

	if(_useAutoProjectLocation)
	{
		Vector3 forward = Vector3(view._13, view._23, view._33).Normalized();
		const Vector3& sceneCenter = sceneBoundBox.GetCenter();

		view._41 = sceneCenter.x - (forward.x * DIRECTIONAL_LIGHT_FRUSTUM_MAX_Z / 2.0f);
		view._42 = sceneCenter.y - (forward.y * DIRECTIONAL_LIGHT_FRUSTUM_MAX_Z / 2.0f);
		view._43 = sceneCenter.z - (forward.z * DIRECTIONAL_LIGHT_FRUSTUM_MAX_Z / 2.0f);
	}

	CameraForm::GetViewMatrix(view, view);

	float orthogonalWH = (_projectionSize < FLT_EPSILON) ? sceneBoundBox.GetSize().Length() : _projectionSize;

	Matrix proj, invNearFarProj;
#if defined(USE_SHADOW_INVERTED_DEPTH)
	Matrix::OrthoLH(proj, orthogonalWH, orthogonalWH, DIRECTIONAL_LIGHT_FRUSTUM_MAX_Z, DIRECTIONAL_LIGHT_FRUSTUM_MIN_Z);
	Matrix::OrthoLH(invNearFarProj, orthogonalWH, orthogonalWH, DIRECTIONAL_LIGHT_FRUSTUM_MIN_Z, DIRECTIONAL_LIGHT_FRUSTUM_MAX_Z);
#else
	Matrix::OrthoLH(proj, orthogonalWH, orthogonalWH, DIRECTIONAL_LIGHT_FRUSTUM_MIN_Z, DIRECTIONAL_LIGHT_FRUSTUM_MAX_Z);
	Matrix::OrthoLH(invNearFarProj, orthogonalWH, orthogonalWH, DIRECTIONAL_LIGHT_FRUSTUM_MAX_Z, DIRECTIONAL_LIGHT_FRUSTUM_MIN_Z);
#endif

	_invNearFarViewProjMat	= view * invNearFarProj;
	_viewProjMat			= view * proj;

#if defined(USE_SHADOW_INVERTED_DEPTH)
	_frustum.Make(_invNearFarViewProjMat);
#else
	_frustum.Make(_viewProjMat);
#endif

	Matrix invViewProj;
	Matrix::Inverse(invViewProj, _viewProjMat);

	_invViewProjViewportMat = invViewportMat * invViewProj;
}

bool DirectionalLight::Intersect(const Intersection::Sphere &sphere) const
{
	return _frustum.In(sphere.center, sphere.radius);
}

void DirectionalLight::MakeLightBufferElement(std::pair<ushort, ushort>& outDir, Param& dummy) const
{
	const Transform* transform = _owner->GetTransform();

	Transform worldTransform(nullptr);
	transform->FetchWorldTransform(worldTransform);
	const auto& forward = worldTransform.GetForward();
	
	outDir.first	= Math::Common::FloatToHalf(forward.x);
	outDir.second	= Math::Common::FloatToHalf(forward.y);
}

Core::Component* DirectionalLight::Clone() const
{
	return new DirectionalLight(*this);
}