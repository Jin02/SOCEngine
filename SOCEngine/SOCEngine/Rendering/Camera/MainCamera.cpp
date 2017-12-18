#include "MainCamera.h"
#include "Utility.hpp"
#include "Matrix.h"
#include "LightCullingUtility.h"
#include "CameraManager.h"
#include "ObjectManager.h"
#include "MeshUtility.h"

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
		auto size			= rect.size.Cast<float>();
		_desc.aspect		= size.w / size.h;
		_desc.renderRect	= rect;
	}

	_camCB.Initialize(dx);
}

bool MainCamera::UpdateCB(Device::DirectX & dx, const Core::Transform& dirtyTransform)
{
	assert(dirtyTransform.GetObjectID() == _objID);

	bool changedTF = dirtyTransform.GetDirty() | _dirty;
	if (changedTF)
		_camCBChangeState = TransformCB::ChangeState::HasChanged;

	if ((_camCBChangeState != TransformCB::ChangeState::No) == false)
		return false;

	_worldMat = dirtyTransform.GetWorldMatrix();
	{
		_camCBData.viewMat			= Matrix::ComputeViewMatrix(_worldMat);
		Matrix& viewMat				= _camCBData.viewMat;
		
		_projMat					= ComputePerspectiveMatrix(true);
		_viewProjMat				= viewMat * _projMat;

		_camCBData.viewProjMat		= _viewProjMat;

		_camCBData.worldPos			= Vector3(_worldMat._41, _worldMat._42, _worldMat._43);
		_camCBData.prevViewProjMat	= _prevViewProjMat;
		_camCBData.packedCamNearFar	= (Half(_desc.near).GetValue() << 16) | Half(_desc.far).GetValue();
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

	return true;
}

Math::Matrix MainCamera::ComputePerspectiveMatrix(bool isInverted) const
{
	float fovRadian = DEG_2_RAD(_desc.fieldOfViewDegree);

	float near	= _desc.near;
	float far	= _desc.far;

	if (isInverted)
		std::swap(near, far);

	return Matrix::PerspectiveFovLH(_desc.aspect, fovRadian, near, far);
}

Math::Matrix MainCamera::ComputeOrthogonalMatrix(bool isInverted) const
{
	float near	= _desc.near;
	float far	= _desc.far;

	if (isInverted)
		std::swap(near, far);

	auto size = _desc.renderRect.size.Cast<float>();
	return Matrix::OrthoLH(size.w, size.h, near, far);
}

void MainCamera::ClassifyMesh(	const MeshPoolPtrs&& pool,
								const ObjectManager& objMgr,
								const TransformPool& transformPool)
{
	// TODO:
	// 일단 Frustum Culling만 수행한다.
	// 나중에 Hierarchical Z Buffer Occlusion Culling(#73)도
	// 이곳에서 추가해야한다

	// Tarnsparent Meshes
	{
		//camCBData was transposed.
		Vector3 viewDir = Vector3(	_camCBData.viewMat._31,
									_camCBData.viewMat._32,
									_camCBData.viewMat._33	);

		MeshUtility::ClassifyTransparentMesh(
			_transparentMeshRenderQ, viewDir, *pool.transparentMeshes, objMgr, transformPool,
			[&thisCam = (*this)](const Mesh& mesh, const Transform& transform)
			{
				Vector3 worldPos = transform.GetWorldPosition();
				return thisCam._frustum.In(worldPos, mesh.GetRadius());
			}
		);
	}

	// Opaque / AlphaTest Meshes
	{
		MeshUtility::ClassifyOpaqueMesh(
			_opaqueMeshRenderQ, *pool.opaqueMeshes, objMgr, transformPool,
			[&frustum = _frustum](const Mesh& mesh, const Transform& tf)
			{
				return frustum.In(tf.GetWorldPosition(), mesh.GetRadius());
			}
		);

		MeshUtility::ClassifyOpaqueMesh(
			_alphaTestMeshRenderQ, *pool.alphaTestMeshes, objMgr, transformPool,
			[&frustum = _frustum](const Mesh& mesh, const Transform& tf)
			{
				return frustum.In(tf.GetWorldPosition(), mesh.GetRadius());
			}
		);
	}
}
