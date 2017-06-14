#include "MainCamera.h"
#include "EngineShaderFactory.hpp"
#include "Utility.hpp"
#include "Matrix.h"
#include "LightCullingUtility.h"

using namespace Utility;
using namespace Device;
using namespace Core;
using namespace Rendering;
using namespace Rendering::Buffer;
using namespace Rendering::Shader;
using namespace Rendering::Camera;
using namespace Device;
using namespace Math;
using namespace Rendering::Manager;
using namespace Rendering::Geometry;

void MainCamera::Initialize(DirectX& dx, ShaderManager& shaderMgr, const Rect<uint>& rect)
{
	// setting desc
	{
		auto size = rect.size.Cast<float>();
		_desc.aspect = size.w / size.h;
		_desc.renderRect = rect;
	}

	_blendedDepthLC.Initialize(dx, shaderMgr, rect.size);

	// Load Shader
	{
		std::vector<Shader::ShaderMacro> macros{dx.GetMSAAShaderMacro(),
												ShaderMacro("USE_COMPUTE_SHADER"),
												ShaderMacro("ENABLE_BLEND")};

		Factory::EngineShaderFactory factory(&shaderMgr);
		ComputeShader shader = *factory.LoadComputeShader(dx, "TBDR", "TileBasedDeferredShadingCS", &macros, "@TBDR");

		Size<uint> size = Light::CullingUtility::ComputeThreadGroupSize(rect.size);
		ComputeShader::ThreadGroup threadGroup(size.w, size.h, 1);
		shader.SetThreadGroupInfo(threadGroup);
	}

	// Light Buffer
	{
		auto size = dx.GetBackBufferSize().Cast<uint>();
		{
			if (dx.GetMSAADesc().Count > 1)
			{
				size.w *= 2;
				size.h *= 2;
			}
		}

		_diffuseLightBuffer.Initialize(dx, size, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, D3D11_BIND_UNORDERED_ACCESS, 1);
		_specularLightBuffer.Initialize(dx, size, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, D3D11_BIND_UNORDERED_ACCESS, 1);
	}

	// setting gbuffer, render target
	{
		const auto& size = _desc.renderRect.size;

		_gbuffer.albedo_occlusion.Initialize(dx, size, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN, 0);
		_gbuffer.normal_roughness.Initialize(dx, size, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN, 0);
		_gbuffer.velocity_metallic_specularity.Initialize(dx, size, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN, 0);

		_gbuffer.emission_materialFlag.Initialize(dx, size, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_UNKNOWN, 0);

		_gbuffer.opaqueDepthBuffer.Initialize(dx, size, true);
		_gbuffer.blendedDepthBuffer.Initialize(dx, size, true);
	}

	_camCB.Initialize(dx);
	_tbrCB.Initialize(dx);

	_Initialized = true;
}

void MainCamera::UpdateCB(Device::DirectX & dx, const Core::Transform& dirtyTransform, uint packedNumOfLights)
{
	assert(dirtyTransform.GetObjectId() == _objId);

	bool changedTF = dirtyTransform.GetDirty() | _dirty;
	if (changedTF)
		_camCBChangeState = TransformCB::ChangeState::HasChanged;

	if ((_camCBChangeState != TransformCB::ChangeState::No) == false)
		return;

	const Matrix& worldMat = dirtyTransform.GetWorldMatrix();

	{
		_camCBData.viewMat = Matrix::ComputeViewMatrix(worldMat);
		Matrix& viewMat = _camCBData.viewMat;
		
		Matrix projMat = ComputePerspectiveMatrix(true);

		_viewProjMat = viewMat * projMat;
		_camCBData.viewProjMat = _viewProjMat;

		_camCBData.worldPos = Vector3(worldMat._41, worldMat._42, worldMat._43);
		_camCBData.prevViewProjMat = _prevViewProjMat;
		_camCBData.packedCamNearFar = (Half(_desc.near).GetValue() << 16) | Half(_desc.far).GetValue();

		_tbrCBData.invProjMat = Matrix::Inverse(projMat);
		_tbrCBData.invViewProjMat = Matrix::Inverse(_viewProjMat);

		Matrix invViewportMat = Matrix::ComputeInvViewportMatrix(_desc.renderRect);
		_tbrCBData.invViewProjViewport = invViewportMat * _tbrCBData.invViewProjMat;
	}

	// Make Frustum
	if (changedTF)
	{
		Matrix notInvProj = ComputePerspectiveMatrix(false);
		_frustum.Make(_camCBData.viewMat * notInvProj);
	}

	_camCBData.viewMat				= Matrix::Transpose(_camCBData.viewMat);
	_camCBData.viewProjMat			= Matrix::Transpose(_camCBData.viewProjMat);
	_camCBData.prevViewProjMat		= Matrix::Transpose(_camCBData.prevViewProjMat);

	_camCB.UpdateSubResource(dx, _camCBData);

	_tbrCBData.invProjMat			= Matrix::Transpose(_tbrCBData.invProjMat);
	_tbrCBData.invViewProjMat		= Matrix::Transpose(_tbrCBData.invViewProjMat);
	_tbrCBData.invViewProjViewport	= Matrix::Transpose(_tbrCBData.invViewProjViewport);

	// packed tbr cb data
	{
		auto& packed = _tbrCBData.packedParam;
		packed.maxNumOfperLightInTile	= Light::CullingUtility::CalcMaxNumLightsInTile(_desc.renderRect.size.Cast<uint>());
		packed.packedNumOfLights		= packedNumOfLights;

		packed.packedViewportSize		= (_desc.renderRect.x << 16) | _desc.renderRect.y;
	}

	_tbrCB.UpdateSubResource(dx, _tbrCBData);

	_camCBChangeState = TransformCB::ChangeState((static_cast<uint>(_camCBChangeState) + 1) % static_cast<uint>(TransformCB::ChangeState::MAX));
	_prevViewProjMat = _viewProjMat;

	_dirty = false;
}

Math::Matrix MainCamera::ComputePerspectiveMatrix(bool isInverted) const
{
	float fovRadian = DEG_2_RAD(_desc.fieldOfViewDegree);

	float near = _desc.near;
	float far = _desc.far;

	if (isInverted)
		std::swap(near, far);

	return Matrix::PerspectiveFovLH(_desc.aspect, fovRadian, near, far);
}

Math::Matrix Rendering::Camera::MainCamera::ComputeOrthogonalMatrix(bool isInverted) const
{
	float near = _desc.near;
	float far = _desc.far;

	if (isInverted)
		std::swap(near, far);

	auto size = _desc.renderRect.size.Cast<float>();
	return Matrix::OrthoLH(size.w, size.h, near, far);
}

void MainCamera::SortTransparentMeshRenderQueue(const Core::Transform& transform, const Manager::MeshManager & meshMgr, const Core::TransformPool & transformPool)
{
	assert(transform.GetObjectId() == _objId);

	_transparentMeshes.clear();
	const auto& meshes = meshMgr.GetPool<TransparencyTrait>().GetVector();
	for (const auto& mesh : meshes)
		_transparentMeshes.push_back(&mesh);

	Vector3 camPos = transform.GetWorldPosition();
	auto SortingByDistance = [&transformPool, camPos](const Mesh* left, const Mesh* right) -> bool
	{
		float leftDistance = D3D11_FLOAT32_MAX;
		{
			auto id = left->GetObjectId();
			uint findIdx = transformPool.GetIndexer().Find(id.Literal());
			assert(findIdx != TransformPool::IndexerType::FailIndex());

			Vector3 leftPos = transformPool.Get(findIdx).GetWorldPosition();
			leftDistance = Vector3::Distance(leftPos, camPos);
		}

		float rightDistance = D3D11_FLOAT32_MAX;
		{
			auto id = right->GetObjectId();
			uint findIdx = transformPool.GetIndexer().Find(id.Literal());
			assert(findIdx != TransformPool::IndexerType::FailIndex());

			Vector3 rightPos = transformPool.Get(findIdx).GetWorldPosition();
			rightDistance = Vector3::Distance(rightPos, camPos);
		}

		return leftDistance < rightDistance;
	};

	std::sort(_transparentMeshes.begin(), _transparentMeshes.end(), SortingByDistance);
}