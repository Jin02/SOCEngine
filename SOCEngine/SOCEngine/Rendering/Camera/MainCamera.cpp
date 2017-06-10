#include "MainCamera.h"
#include "EngineShaderFactory.hpp"
#include "Utility.hpp"
#include "Matrix.h"

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

void MainCamera::Initialize(DirectX& dx, ShaderManager& shaderMgr)
{
	std::vector<Shader::ShaderMacro> macros;
	{
		ShaderMacro msaaMacro = dx.GetMSAAShaderMacro();
		macros.push_back(msaaMacro);

		macros.push_back(ShaderMacro("USE_COMPUTE_SHADER", ""));
		macros.push_back(ShaderMacro("ENABLE_BLEND", ""));
	}

	// Load Shader
	{
		Factory::EngineShaderFactory factory(&shaderMgr);
		_tbrShader = *factory.LoadComputeShader(dx, "TBDR", "TileBasedDeferredShadingCS", &macros, "@TBDR");

		Size<uint> size = ComputeThreadGroupSize(dx.GetBackBufferSize());
		ComputeShader::ThreadGroup threadGroup(size.w, size.h, 1);
		_tbrShader.SetThreadGroupInfo(threadGroup);
	}

	Size<float> fltSize = Size<float>(	static_cast<float>(dx.GetBackBufferSize().w),
										static_cast<float>(dx.GetBackBufferSize().h)	);
	Rect<float> renderRect = Rect<float>(0.0f, 0.0f, fltSize.w, fltSize.h);

	// setting desc
	{
		_desc.aspect = renderRect.size.w / renderRect.size.h;
		_desc.renderRect = renderRect;
	}

	// setting gbuffer, render target
	{
		Size<uint> size(static_cast<uint>(renderRect.size.w), static_cast<uint>(renderRect.size.h));
		uint mipLevel = static_cast<uint>(log(max(renderRect.size.w, renderRect.size.h)) / log(2.0f)) + 1;
		_renderTarget.Initialize(dx, size, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, 0, 0, mipLevel);

		_gbuffer.albedo_occlusion.Initialize(dx, size, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN, 0);
		_gbuffer.normal_roughness.Initialize(dx, size, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN, 0);
		_gbuffer.velocity_metallic_specularity.Initialize(dx, size, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN, 0);

		_gbuffer.emission_materialFlag.Initialize(dx, size, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_UNKNOWN, 0);

		_gbuffer.opaqueDepthBuffer.Initialize(dx, size, true);
		_gbuffer.blendedDepthBuffer.Initialize(dx, size, true);
	}

	_camCB.Initialize(dx);

	_Initialized = true;
}

void MainCamera::UpdateCB(Device::DirectX & dx, const Core::Transform& dirtyTransform)
{
	assert(dirtyTransform.GetObjectId() == _objId);

	bool changedTF = dirtyTransform.GetDirty() | _dirty;
	if (changedTF)
		_camCBChangeState = TransformCB::ChangeState::HasChanged;

	if ((_camCBChangeState != TransformCB::ChangeState::No) == false)
		return;

	const Matrix& worldMat = dirtyTransform.GetWorldMatrix();

	CameraCBData cbData;
	{
		cbData.viewMat = ComputeViewMatrix(worldMat);
		Matrix& viewMat = cbData.viewMat;
		
		Matrix projMat = ComputePerspectiveMatrix(true);

		_viewProjMat = viewMat * projMat;
		cbData.viewProjMat = _viewProjMat;

		cbData.worldPos = Vector3(worldMat._41, worldMat._42, worldMat._43);
		cbData.prevViewProjMat = _prevViewProjMat;
		cbData.packedCamNearFar = (Half(_desc.near).GetValue() << 16) | Half(_desc.far).GetValue();
	}

	// Make Frustum
	if (changedTF)
	{
		Matrix notInvProj = ComputePerspectiveMatrix(false);
		_frustum.Make(cbData.viewMat * notInvProj);
	}

	cbData.viewMat			= Matrix::Transpose(cbData.viewMat);
	cbData.viewProjMat		= Matrix::Transpose(cbData.viewProjMat);
	cbData.prevViewProjMat	= Matrix::Transpose(cbData.prevViewProjMat);

	_camCB.UpdateSubResource(dx, cbData);

	_camCBChangeState = TransformCB::ChangeState((static_cast<uint>(_camCBChangeState) + 1) % static_cast<uint>(TransformCB::ChangeState::MAX));
	_prevViewProjMat = _viewProjMat;

	_dirty = false;
}

const Size<uint> MainCamera::ComputeThreadGroupSize(const Size<uint>& size) const
{
	return Size<uint>(
		static_cast<uint>((size.w + LIGHT_CULLING_TILE_RES - 1) / static_cast<float>(LIGHT_CULLING_TILE_RES)),
		static_cast<uint>((size.h + LIGHT_CULLING_TILE_RES - 1) / static_cast<float>(LIGHT_CULLING_TILE_RES))
		);
}

uint Rendering::Camera::MainCamera::CalcMaxNumLightsInTile(const Size<uint>& size) const
{
	const uint key = LIGHT_CULLING_TILE_RES;
	return (LIGHT_CULLING_LIGHT_MAX_COUNT_IN_TILE - (key * (size.h / 120)));
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

	return Matrix::OrthoLH(_desc.renderRect.size.w, _desc.renderRect.size.h, near, far);
}

Math::Matrix Rendering::Camera::MainCamera::ComputeViewMatrix(const Math::Matrix & worldMatrix)
{
	Math::Matrix outMatrix = worldMatrix;

	Vector3 worldPos;
	worldPos.x = worldMatrix._41;
	worldPos.y = worldMatrix._42;
	worldPos.z = worldMatrix._43;

	Math::Vector3 right = Math::Vector3(worldMatrix._11, worldMatrix._21, worldMatrix._31);
	Math::Vector3 up = Math::Vector3(worldMatrix._12, worldMatrix._22, worldMatrix._32);
	Math::Vector3 forward = Math::Vector3(worldMatrix._13, worldMatrix._23, worldMatrix._33);

	Vector3 p;
	p.x = -Vector3::Dot(right, worldPos);
	p.y = -Vector3::Dot(up, worldPos);
	p.z = -Vector3::Dot(forward, worldPos);

	outMatrix._41 = p.x;
	outMatrix._42 = p.y;
	outMatrix._43 = p.z;
	outMatrix._44 = 1.0f;

	return outMatrix;
}

Math::Matrix Rendering::Camera::MainCamera::ComputeViewportMatrix(const Rect<float>& rect)
{
	Math::Matrix outMat;

	outMat._11 = rect.size.w / 2.0f;
	outMat._12 = 0.0f;
	outMat._13 = 0.0f;
	outMat._14 = 0.0f;

	outMat._21 = 0.0f;
	outMat._22 = -rect.size.h / 2.0f;
	outMat._23 = 0.0f;
	outMat._24 = 0.0f;

	outMat._31 = 0.0f;
	outMat._32 = 0.0f;
	outMat._33 = 1.0f;
	outMat._34 = 0.0f;

	outMat._41 = rect.x + rect.size.w / 2.0f;
	outMat._42 = rect.y + rect.size.h / 2.0f;
	outMat._43 = 0.0f;
	outMat._44 = 1.0f;

	return outMat;
}

Math::Matrix Rendering::Camera::MainCamera::ComputeInvViewportMatrix(const Rect<float>& rect)
{
	Math::Matrix viewportMat = ComputeViewportMatrix(rect);
	return Math::Matrix::Inverse(viewportMat);
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