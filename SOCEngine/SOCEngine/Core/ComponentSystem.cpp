#include "ComponentSystem.hpp"
#include "MeshUtility.h"

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
	const TransformPool& transformPool,
	const ObjectID::IndexHashMap& lightShaftIndexer)
{
	ShadowManager& shadowMgr = GetManager_Direct<ShadowManager>();
	LightManager& lightMgr = GetManager_Direct<LightManager>();
	MeshManager& meshMgr = GetManager_Direct<MeshManager>();
	CameraManager& camMgr = GetManager_Direct<CameraManager>();

	// Check Dirty
	{
		lightMgr.CheckDirtyLights(transformPool);
		shadowMgr.CheckDirtyShadows(lightMgr, transformPool);
		meshMgr.CheckDirty(transformPool);
		meshMgr.UpdateTraits();
	}

	bool isDirtyMesh = meshMgr.GetHasDirtyMeshes();
	if (meshMgr.GetHasDirtyMeshes())
	{
		uint	value = 0xff7fffff; float fltMin = (*(float*)&value);
		value = 0x7f7fffff;	float fltMax = (*(float*)&value);

		Vector3 worldMin = Vector3(fltMax, fltMax, fltMax);
		Vector3 worldMax = Vector3(fltMin, fltMin, fltMin);
		meshMgr.ComputeWorldSize(worldMin, worldMax, transformPool);
		_sceneBoundBox.SetMinMax(worldMin, worldMax);
	}

	meshMgr.UpdateTransformCB(dx, transformPool);

	// Update MainCamera
	{
		auto& mainCamera = camMgr.GetMainCamera();
		auto transform = transformPool.Find(mainCamera.GetObjectID().Literal());
		assert(transform);

		mainCamera.UpdateCB(dx, *transform);
		mainCamera.SortTransparentMeshRenderQueue(*transform, meshMgr, transformPool);
		MeshUtility::Culling(mainCamera.GetFrustum(), meshMgr, transformPool);
	}


	lightMgr.UpdateTransformBuffer(transformPool);
	lightMgr.UpdateParamBuffer(shadowMgr, lightShaftIndexer);
	lightMgr.UpdateSRBuffer(dx);

	shadowMgr.UpdateGlobalCB(dx);
	shadowMgr.UpdateConstBuffer(dx);
	shadowMgr.UpdateBuffer(lightMgr, transformPool, _sceneBoundBox);
	shadowMgr.UpdateSRBuffer(dx);

	// Clear Dirty
	{
		lightMgr.ClearDirty();
		shadowMgr.ClearDirty();
		meshMgr.ClearDirty();
	}
}
