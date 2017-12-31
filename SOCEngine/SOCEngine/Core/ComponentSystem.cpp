#include "ComponentSystem.h"
#include "MeshUtility.h"
#include "ShadowAtlasMapRenderer.h"

using namespace Device;
using namespace Rendering;
using namespace Core;
using namespace Rendering::Light;
using namespace Rendering::Light::Buffer;
using namespace Rendering::Geometry;
using namespace Rendering::Shadow;
using namespace Rendering::Camera;
using namespace Rendering::Manager;
using namespace Rendering::Renderer;
using namespace Math;
using namespace Intersection;

void ComponentSystem::Initialize(DirectX& dx)
{
	GetManager_Direct<LightManager>().Initialize(dx);
	GetManager_Direct<ShadowManager>().Initialize(dx);
}

void ComponentSystem::UpdateBuffer(DirectX& dx,
	const TransformPool& transformPool,
	const ObjectManager& objectManager,
	const ShadowAtlasMapRenderer& shadowAtlasMapRenderer)
{
	ShadowManager& shadowMgr	= GetManager_Direct<ShadowManager>();
	LightManager& lightMgr		= GetManager_Direct<LightManager>();
	MeshManager& meshMgr		= GetManager_Direct<MeshManager>();
	CameraManager& camMgr		= GetManager_Direct<CameraManager>();

	// Check Dirty
	{
		lightMgr.CheckDirtyLights(transformPool);
		shadowMgr.CheckDirtyWithCullShadows(camMgr, objectManager, lightMgr, transformPool);
		meshMgr.CheckDirty(transformPool, objectManager, camMgr);
	}

	if (meshMgr.GetHasDirtyMeshes())
	{
		uint value = 0xff7fffff;
		float fltMin = (*(float*)&value);
		
		value = 0x7f7fffff;
		float fltMax = (*(float*)&value);

		Vector3 worldMin = Vector3(fltMax, fltMax, fltMax);
		Vector3 worldMax = Vector3(fltMin, fltMin, fltMin);
		meshMgr.ComputeWorldSize(worldMin, worldMax, transformPool);
		_sceneBoundBox.SetMinMax(worldMin, worldMax);
	}

	meshMgr.UpdateTransformCB(dx, transformPool);

	// Update MainCamera
	{
		auto transform = transformPool.Find(camMgr.GetMainCamera().GetObjectID().Literal());
		assert(transform);

		camMgr.GetMainCamera().UpdateCB(dx, *transform);
		camMgr.GetMainCamera().ClassifyMesh(meshMgr.GetAllMeshPoolPtrs(), objectManager, transformPool);
	}

	lightMgr.UpdateTransformBuffer(transformPool);
	lightMgr.UpdateParamBuffer(shadowMgr);
	lightMgr.UpdateSRBuffer(dx);

	shadowMgr.UpdateBuffer(lightMgr, transformPool, _sceneBoundBox);
	shadowMgr.UpdateConstBuffer(dx, shadowAtlasMapRenderer);
	shadowMgr.UpdateSRBuffer(dx);
}

void ComponentSystem::ClearDirty()
{
	GetManager_Direct<ShadowManager>().ClearDirty();
	GetManager_Direct<LightManager>().ClearDirty();
	GetManager_Direct<MeshManager>().ClearDirty();
	GetManager_Direct<CameraManager>().ClearDirty();
}

void ComponentSystem::Destroy()
{

}
