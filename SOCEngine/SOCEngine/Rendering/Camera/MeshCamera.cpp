#include "MeshCamera.h"
#include "Object.hpp"
#include <algorithm>

using namespace Math;
using namespace std;
using namespace Rendering;
using namespace Rendering::Buffer;
using namespace Intersection;
using namespace Device;
using namespace Core;
using namespace Rendering::Camera;
using namespace Rendering::Manager;
using namespace Rendering::Geometry;

MeshCamera::MeshCamera(Core::ObjectId id)
	: _id(id)
{
}

void MeshCamera::Initialize(DirectX& dx, const Rect<float>& renderRect, bool useMipmap)
{
	_param = std::make_unique<Param>();
	_param->aspect = renderRect.size.w / renderRect.size.h;

	uint mipLevel = useMipmap ? static_cast<uint>(log(max(renderRect.size.w, renderRect.size.h)) / log(2.0f)) + 1 : 1;

	Size<uint> size(static_cast<uint>(renderRect.size.w), static_cast<uint>(renderRect.size.h));
	_renderTarget.Initialize(dx, size, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, 0, 0, mipLevel);

	_param->renderRect = renderRect;
	_camCB.Initialize(dx);

	_gbuffer.albedo_occlusion.Initialize(dx, size, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN, 0);
	_gbuffer.normal_roughness.Initialize(dx, size, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN, 0);
	_gbuffer.velocity_metallic_specularity.Initialize(dx, size, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN, 0);

	_gbuffer.emission_materialFlag.Initialize(dx, size, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_UNKNOWN, 0);

	_gbuffer.opaqueDepthBuffer.Initialize(dx, size, true);
	_gbuffer.blendedDepthBuffer.Initialize(dx, size, true);
}

void MeshCamera::ComputePerspectiveMatrix(Math::Matrix &outMatrix, bool isInverted) const
{
	float fovRadian = DEG_2_RAD(_param->fieldOfViewDegree);

	float near	= _param->near;
	float far	= _param->far;

	if(isInverted)
		std::swap(near, far);

	Matrix::PerspectiveFovLH(outMatrix, _param->aspect, fovRadian, near, far);
}

void MeshCamera::ComputeOrthogonalMatrix(Math::Matrix &outMatrix, bool isInverted) const
{
	float near = _param->near;
	float far = _param->far;

	if (isInverted)
		std::swap(near, far);

	Matrix::OrthoLH(outMatrix, _param->renderRect.size.w, _param->renderRect.size.h, near, far);
}

void MeshCamera::ComputeViewMatrix(Math::Matrix &outMatrix, const Math::Matrix &worldMatrix)
{
	outMatrix = worldMatrix;

	Vector3 worldPos;
	worldPos.x = worldMatrix._41;
	worldPos.y = worldMatrix._42;
	worldPos.z = worldMatrix._43;

	Math::Vector3 right		= Math::Vector3(worldMatrix._11, worldMatrix._21, worldMatrix._31);
	Math::Vector3 up		= Math::Vector3(worldMatrix._12, worldMatrix._22, worldMatrix._32);
	Math::Vector3 forward	= Math::Vector3(worldMatrix._13, worldMatrix._23, worldMatrix._33);

	Vector3 p;
	p.x = -Vector3::Dot(right,		worldPos);
	p.y = -Vector3::Dot(up,			worldPos);
	p.z = -Vector3::Dot(forward,	worldPos);

	outMatrix._41 = p.x;
	outMatrix._42 = p.y;
	outMatrix._43 = p.z;
	outMatrix._44 = 1.0f;
}

void MeshCamera::ComputeViewportMatrix(Math::Matrix& outMat, const Rect<float>& rect)
{
	outMat._11 = rect.size.w /  2.0f;
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
}

void MeshCamera::ComputeInvViewportMatrix(Math::Matrix& outMat, const Rect<float>& rect)
{
	Math::Matrix viewportMat;
	ComputeViewportMatrix(viewportMat, rect);

	Math::Matrix::Inverse(outMat, viewportMat);
}

void MeshCamera::UpdateCB(DirectX& dx, const Transform& transform)
{
	assert(transform.GetObjectId() == _id);
	
	bool changedTF = transform.GetDirty() || _dirty;
	if(changedTF)
		_camCBChangeState = TransformCB::ChangeState::HasChanged;

	bool isUpdate = (_camCBChangeState != TransformCB::ChangeState::No);
	if(isUpdate == false)
		return;

	const Matrix& worldMat = transform.GetWorldMatrix();

	CameraCBData cbData;
	{
		Matrix& viewMat = cbData.viewMat;
		ComputeViewMatrix(cbData.viewMat, worldMat);

		Matrix projMat;
		ComputePerspectiveMatrix(projMat, true);

		_viewProjMat = viewMat * projMat;
		cbData.viewProjMat = _viewProjMat;

		cbData.worldPos = Vector3(worldMat._41, worldMat._42, worldMat._43);
		cbData.prevViewProjMat = _prevViewProjMat;
		cbData.packedCamNearFar = (Half(_param->near).GetValue() << 16) | Half(_param->far).GetValue();
	}

	// Make Frustum
	if(changedTF)
	{
		Matrix notInvProj;
		ComputePerspectiveMatrix(notInvProj, false);
		_param->frustum.Make(cbData.viewMat * notInvProj);
	}

	Matrix::Transpose(cbData.viewMat, cbData.viewMat);
	Matrix::Transpose(cbData.viewProjMat, cbData.viewProjMat);
	Matrix::Transpose(cbData.prevViewProjMat, cbData.prevViewProjMat);

	_camCB.UpdateSubResource(dx, cbData);
	
	_camCBChangeState = TransformCB::ChangeState( (static_cast<uint>(_camCBChangeState) + 1) % static_cast<uint>(TransformCB::ChangeState::MAX) );
	_prevViewProjMat = _viewProjMat;
}

void MeshCamera::SortTransparentMeshRenderQueue(const Transform& transform, const MeshManager& meshMgr, const TransformPool& transformPool)
{
	assert(transform.GetObjectId() == _id);

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
			uint findIdx = transformPool.GetIndexer().Find(id);			
			assert(findIdx != TransformPool::IndexerType::FailIndex());

			Vector3 leftPos = transformPool.Get(findIdx).GetWorldPosition();
			leftDistance = Vector3::Distance(leftPos, camPos);
		}

		float rightDistance = D3D11_FLOAT32_MAX;
		{
			auto id = right->GetObjectId();
			uint findIdx = transformPool.GetIndexer().Find(id);
			assert(findIdx != TransformPool::IndexerType::FailIndex());

			Vector3 rightPos = transformPool.Get(findIdx).GetWorldPosition();
			rightDistance = Vector3::Distance(rightPos, camPos);
		}

		return leftDistance < rightDistance;
	};

	std::sort(_transparentMeshes.begin(), _transparentMeshes.end(), SortingByDistance);
}