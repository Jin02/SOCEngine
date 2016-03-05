#include "ReflectionProbe.h"
#include "MathCommon.h"
#include "Transform.h"
#include "CameraForm.h"
#include "Object.h"
#include "BindIndexInfo.h"
#include "Scene.h"

using namespace Core;
using namespace Math;
using namespace Rendering;
using namespace Rendering::Camera;
using namespace Rendering::Texture;
using namespace Rendering::Buffer;
using namespace Rendering::Manager;
using namespace Rendering::Shadow;

#define REFLECTION_PROBE_SIZE 512.0f

ReflectionProbe::ReflectionProbe() : Component(), _cubeMap(nullptr), _viewProjMatCB(nullptr)
{
}

ReflectionProbe::~ReflectionProbe()
{
	SAFE_DELETE(_cubeMap);
	SAFE_DELETE(_viewProjMatCB);
}

void ReflectionProbe::OnInitialize()
{
	// CubeMap
	{
		_cubeMap = new TextureCube;
	
		const Size<uint> size = Size<uint>((uint)REFLECTION_PROBE_SIZE, (uint)REFLECTION_PROBE_SIZE);
		_cubeMap->Initialize(size, DXGI_FORMAT_R8G8B8A8_UNORM, true, true, true);
	}

	// view proj cb
	{
		_viewProjMatCB = new ConstBuffer;
		_viewProjMatCB->Initialize(sizeof(Matrix) * 6);
	}
}

void ReflectionProbe::OnDestroy()
{
	_cubeMap->Destroy();
	_viewProjMatCB->Destory();
}

void ReflectionProbe::OnUpdateTransformCB(const Device::DirectX*& dx, const Rendering::TransformCB& transformCB)
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
		Matrix::Transpose(outViewProj, outViewProj);
	};

	float projNear	= _range;
	float projFar	= _projNear;

	Matrix proj;
	Matrix::PerspectiveFovLH(proj, 1.0f, Common::Deg2Rad(90.0f), projNear, projFar);

	Vector3 worldPos;
	_owner->GetTransform()->FetchWorldPosition(worldPos);

	Matrix viewProjs[6];
	Matrix& frontZViewProj = viewProjs[0];
	ComputeViewProj(frontZViewProj, worldPos, forwards[0], ups[0], proj);

	bool isDifferent = memcmp(&_prevFrontZViewProjMat, &frontZViewProj, sizeof(Matrix)) != 0;
	if(isDifferent == false)
		return;

	_prevFrontZViewProjMat = frontZViewProj;

	for(uint i=1; i<6; ++i)
		ComputeViewProj(viewProjs[i], worldPos, forwards[i], ups[i], proj);

	_viewProjMatCB->UpdateSubResource(dx->GetContext(), &viewProjs);
}

void ReflectionProbe::Render(const Device::DirectX*& dx, const Core::Scene* scene)
{
	ID3D11DeviceContext* context = dx->GetContext();

	ID3D11RenderTargetView* rtv = _cubeMap->GetRenderTargetView();
	context->OMSetRenderTargets(1, &rtv, _cubeMap->GetDepthStencilView());

	D3D11_VIEWPORT viewport;
	{
		viewport.TopLeftX	= 0.0f;
		viewport.TopLeftY	= 0.0f;
		viewport.MinDepth	= 0.0f;
		viewport.MaxDepth	= 1.0f;
		viewport.Width		= REFLECTION_PROBE_SIZE;
		viewport.Height		= REFLECTION_PROBE_SIZE;
	}
	context->RSSetViewports(1, &viewport);

	_cubeMap->Clear(dx);

	ID3D11Buffer* buffer = _viewProjMatCB->GetBuffer();
	context->GSSetConstantBuffers(uint(ConstBufferBindIndex::ReflectionProbe_ViewProjs), 1, &buffer);

	// Render Scene
	{
		auto Intersect = [&](const Intersection::Sphere& sphere) -> bool
		{
			float squaredDistance = 0.0f;
			{
				auto Check = [](float p, float bMin, float bMax) -> float
				{
					float out	= 0.0f;
					float v		= p;

					if(v < bMin)
					{
						float val = bMin - v;
						out += val * val;
					}
					if(v > bMax)
					{
						float val = v - bMax;
						out += val * val;
					}

					return out;
				};

				const Vector3& bMin = _boundBox.GetMin();
				const Vector3& bMax = _boundBox.GetMax();

				squaredDistance += Check(sphere.center.x, bMin.x, bMax.x);
				squaredDistance += Check(sphere.center.y, bMin.y, bMax.y);
				squaredDistance += Check(sphere.center.z, bMin.z, bMax.z);
			}

			return squaredDistance <= (sphere.radius * sphere.radius);
		};
		std::function<bool(const Intersection::Sphere&)> intersectFunc = Intersect;

		const RenderManager* renderManager	= scene->GetRenderManager();

		const LightManager* lightManager	= scene->GetLightManager();
		lightManager->BindResources(dx, false, false, true);

		const ShadowRenderer* shadowManager	= scene->GetShadowManager();
		shadowManager->BindResources(dx, false, false, true);

		// Opaque
		{
			const auto& opaqueMeshes = renderManager->GetOpaqueMeshes();
			MeshCamera::RenderMeshesUsingSortedMeshVectorByVB(dx, renderManager, opaqueMeshes, RenderType::Forward_ReflectionProbe, nullptr, &intersectFunc);
		}

		// AlphaTest
		{
			const auto& alphaTestMeshes = renderManager->GetAlphaTestMeshes();
			MeshCamera::RenderMeshesUsingSortedMeshVectorByVB(dx, renderManager, alphaTestMeshes, RenderType::Forward_ReflectionProbeWithAlphaTest, nullptr, &intersectFunc);
		}

		//const auto& transparencyMeshes = renderManager->GetTransparentMeshes();
		//MeshCamera::RenderMeshesUsingSortedMeshVectorByVB(dx, renderManager, alphaTestMeshes, RenderType::Forward_ReflectionProbeTransparency, nullptr, &intersectFunc);

		lightManager->UnbindResources(dx, false, false, true);
		shadowManager->UnbindResources(dx, false, false, true);
	}

	buffer = nullptr;
	context->GSSetConstantBuffers(uint(ConstBufferBindIndex::ReflectionProbe_ViewProjs), 1, &buffer);
}