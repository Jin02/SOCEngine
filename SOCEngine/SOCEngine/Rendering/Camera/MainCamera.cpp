#include "MainCamera.h"
#include "Utility.hpp"
#include "Matrix.h"
#include "LightCullingUtility.h"
#include "CameraManager.h"

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

	_camCB.Initialize(dx);
}

bool MainCamera::UpdateCB(Device::DirectX & dx, const Core::Transform& dirtyTransform)
{
	assert(dirtyTransform.GetObjectId() == _objId);

	bool changedTF = dirtyTransform.GetDirty() | _dirty;
	if (changedTF)
		_camCBChangeState = TransformCB::ChangeState::HasChanged;

	if ((_camCBChangeState != TransformCB::ChangeState::No) == false)
		return false;

	const Matrix& worldMat = dirtyTransform.GetWorldMatrix();

	{
		_camCBData.viewMat = Matrix::ComputeViewMatrix(worldMat);
		Matrix& viewMat = _camCBData.viewMat;
		
		_projMat = ComputePerspectiveMatrix(true);
		_viewProjMat = viewMat * _projMat;

		_camCBData.viewProjMat = _viewProjMat;

		_camCBData.worldPos = Vector3(worldMat._41, worldMat._42, worldMat._43);
		_camCBData.prevViewProjMat = _prevViewProjMat;
		_camCBData.packedCamNearFar = (Half(_desc.near).GetValue() << 16) | Half(_desc.far).GetValue();
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

	_camCBChangeState = TransformCB::ChangeState((static_cast<uint>(_camCBChangeState) + 1) % static_cast<uint>(TransformCB::ChangeState::MAX));
	_prevViewProjMat = _viewProjMat;

	_dirty = false;
	return true;
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

	//camCBData was transposed.
	Vector3 viewDir = Vector3(	_camCBData.viewMat._31,
								_camCBData.viewMat._32,
								_camCBData.viewMat._33	);

	//	Vector3 camPos = transform.GetWorldPosition();
	auto SortingByDistance = [&transformPool, &viewDir](const Mesh* left, const Mesh* right) -> bool
	{
		auto SortKey = [&viewDir](Vector3 pos) -> float
		{
			return Vector3::Dot(pos, viewDir);
		};

		float leftKey = D3D11_FLOAT32_MAX;
		{
			auto id = left->GetObjectId();
			uint findIdx = transformPool.GetIndexer().Find(id.Literal());
			assert(findIdx != TransformPool::IndexerType::FailIndex());

			Vector3 leftPos = transformPool.Get(findIdx).GetWorldPosition();
			leftKey = SortKey(leftPos);
		}

		float rightKey = D3D11_FLOAT32_MAX;
		{
			auto id = right->GetObjectId();
			uint findIdx = transformPool.GetIndexer().Find(id.Literal());
			assert(findIdx != TransformPool::IndexerType::FailIndex());

			Vector3 rightPos = transformPool.Get(findIdx).GetWorldPosition();
			rightKey = SortKey(rightPos);
		}

		return leftKey < rightKey;
	};

	std::sort(_transparentMeshes.begin(), _transparentMeshes.end(), SortingByDistance);
}
