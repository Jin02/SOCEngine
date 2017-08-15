#include "ComponentSystem.hpp"

using namespace Device;
using namespace Rendering;
using namespace Core;
using namespace Rendering::Light;
using namespace Rendering::Light::Buffer;
using namespace Rendering::Geometry;
using namespace Rendering::Shadow;
using namespace Rendering::Camera;
using namespace Rendering::Manager;
using namespace Math;
using namespace Intersection;

void ComponentSystem::UpdateBuffer(DirectX& dx,
	Vector3& worldMin, Vector3& worldMax,
	const TransformPool& transformPool,
	const ObjectId::IndexHashMap& lightShaftIndexer,
	const BoundBox& sceneBoundBox)
{
	ShadowManager& shadowMgr = GetManager_Direct<ShadowManager>();
	LightManager& lightMgr = GetManager_Direct<LightManager>();
	MeshManager& meshMgr = GetManager_Direct<MeshManager>();
	CameraManager& camMgr = GetManager_Direct<CameraManager>();
	
	// Update MainCamera
	{
		auto& mainCamera = camMgr.GetMainCamera();
		auto transform = transformPool.Find(mainCamera.GetObjectId().Literal());
		assert(transform);

		mainCamera.UpdateCB(dx, *transform);
		mainCamera.SortTransparentMeshRenderQueue(*transform, meshMgr, transformPool);
	}

	// Check Dirty
	{
		lightMgr.CheckDirtyLights(transformPool);
		shadowMgr.CheckDirtyShadows(lightMgr, transformPool);
		meshMgr.UpdateTraits();
	}

	meshMgr.ComputeWorldSize(worldMin, worldMax, transformPool);
	meshMgr.UpdateTransformCB(dx, transformPool);

	lightMgr.UpdateTransformBuffer(transformPool);
	lightMgr.UpdateParamBuffer(shadowMgr, lightShaftIndexer);
	lightMgr.UpdateSRBuffer(dx);

	shadowMgr.UpdateGlobalCB(dx);
	shadowMgr.UpdateConstBuffer(dx);
	shadowMgr.UpdateBuffer(lightMgr, transformPool, sceneBoundBox);
	shadowMgr.UpdateSRBuffer(dx);

	// Clear Dirty
	{
		lightMgr.ClearDirtyLights();
		shadowMgr.ClearDirtyShadows();
	}
}
