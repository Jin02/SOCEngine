#include "EngineUtility.h"
#include "DirectX.h"

#include "Engine.h"
#include "ObjectManager.h"
#include "RenderingSystem.h"

using namespace Core;
using namespace Importer;
using namespace Rendering;
using namespace Rendering::Material;
using namespace Rendering::Camera;
using namespace Rendering::Manager;

EngineUtility::EngineUtility(Engine& e) : _engine(e)
{
}

void EngineUtility::OpaqueMeshToTransparentMesh(ObjectID meshObjID)
{
	auto& meshMgr = _engine.GetComponentSystem().GetManager<Geometry::Mesh>();
	meshMgr.ChangeTrait(meshObjID, *meshMgr.GetOpaqueMeshPool(), *meshMgr.GetTransparentMeshPool());
}

void EngineUtility::OpaqueMeshToAlphaTestMesh(ObjectID meshObjID)
{
	auto& meshMgr = _engine.GetComponentSystem().GetManager<Geometry::Mesh>();
	meshMgr.ChangeTrait(meshObjID, *meshMgr.GetOpaqueMeshPool(), *meshMgr.GetAlphaTestMeshPool());
}

void EngineUtility::AlphaTestMeshToOpaqueMesh(ObjectID meshObjID)
{
	auto& meshMgr = _engine.GetComponentSystem().GetManager<Geometry::Mesh>();
	meshMgr.ChangeTrait(meshObjID, *meshMgr.GetAlphaTestMeshPool(), *meshMgr.GetOpaqueMeshPool());
}

void EngineUtility::AlphaTestMeshToTransparentMesh(ObjectID meshObjID)
{
	auto& meshMgr = _engine.GetComponentSystem().GetManager<Geometry::Mesh>();
	meshMgr.ChangeTrait(meshObjID, *meshMgr.GetAlphaTestMeshPool(), *meshMgr.GetTransparentMeshPool());
}

void EngineUtility::TransparentMeshToOpaqueMesh(ObjectID meshObjID)
{
	auto& meshMgr = _engine.GetComponentSystem().GetManager<Geometry::Mesh>();
	meshMgr.ChangeTrait(meshObjID, *meshMgr.GetTransparentMeshPool(), *meshMgr.GetOpaqueMeshPool());
}

void EngineUtility::TransparentMeshToAlphaTestMesh(ObjectID meshObjID)
{
	auto& meshMgr = _engine.GetComponentSystem().GetManager<Geometry::Mesh>();
	meshMgr.ChangeTrait(meshObjID, *meshMgr.GetTransparentMeshPool(), *meshMgr.GetAlphaTestMeshPool());
}

PhysicallyBasedMaterial EngineUtility::AcquireMaterial(const std::string& name)
{
	PhysicallyBasedMaterial material(name);
	material.Initialize(_engine.GetDirectX());

	return material;
}

MaterialID EngineUtility::AddMaterialToPool(PhysicallyBasedMaterial& pbm)
{
	return _engine.GetRenderingSystem().GetMaterialManager().Add<PhysicallyBasedMaterial>(pbm).first;
}

void EngineUtility::SetVXGICenterPosition(const Math::Vector3& worldPos)
{
	_engine.GetRenderingSystem().GetMainRenderer().GetGlobalIllumination().SetVXGI_VoxelizeCenterPos(worldPos);
}

Object& EngineUtility::AcquireObject(const std::string& name)
{
	return _engine.GetObjectManager().Acquire(name);
}

Object* EngineUtility::FindObject(const std::string& name)
{
	return _engine.GetObjectManager().Find(name);
}

Object* EngineUtility::LoadMesh(const std::string& fileDir, bool useDynamicVB, bool useDynamicIB)
{
	ObjectID id = _engine._importer.Load(
		MeshImporter::ManagerParam{
		_engine._rendering.GetBufferManager(),
		_engine._rendering.GetMaterialManager(),
		_engine._objectManager,
		_engine._transformPool,
		_engine._rendering.GetTexture2DManager(),
		_engine._componentSystem,
		_engine._dx
	},
		fileDir, useDynamicVB, useDynamicIB);

	return (id != ObjectID::Undefined()) ? _engine._objectManager.Find(id) : nullptr;
}

void EngineUtility::SetSkyBoxToMainCamera(MaterialID skyBox)
{
	_engine.GetComponentSystem().GetMainCamera().SetSkyBoxMaterialID(skyBox);
}

void EngineUtility::ActivateSkyBox(const std::string& cubeTexturePath, const std::string& materialKey)
{
	auto cubeMap = LoadTextureFromFile(cubeTexturePath, false);
	assert(cubeMap);

	SkyBoxMaterial skyMat(materialKey);
	skyMat.UpdateCubeMap(*cubeMap);
	skyMat.Initialize(_engine.GetDirectX(), _engine.GetRenderingSystem().GetShaderManager());

	auto key = _engine.GetRenderingSystem().GetMaterialManager().Add(skyMat).first;
	_engine.GetComponentSystem().GetMainCamera().SetSkyBoxMaterialID(key);
}

void EngineUtility::ActivateSkyScattering(uint resolution, const Object& directionalLight)
{
	MaterialID matID = _engine.GetRenderingSystem().ActivateSkyScattering(_engine, resolution, directionalLight);
	_engine.GetComponentSystem().GetMainCamera().SetSkyBoxMaterialID(matID);
}

void EngineUtility::DeactivateSkyScattering()
{
	_engine.GetRenderingSystem().DeactivateSkyScattering();
}

void EngineUtility::SetSunShaftParam(ObjectID directionalLightID, float circleSize, float circleIntensity)
{
	_engine.GetRenderingSystem().GetPostProcessPipeline().SetSunShaftParam(directionalLightID, circleSize, circleIntensity);
}

void EngineUtility::SetUseDoF(bool use)
{
	_engine.GetRenderingSystem().GetPostProcessPipeline().SetUseDoF(use);
}

void EngineUtility::SetUseSSAO(bool use)
{
	_engine.GetRenderingSystem().GetPostProcessPipeline().SetUseSSAO(use);
}

void EngineUtility::SetUseSunShaft(bool use)
{
	_engine.GetRenderingSystem().GetPostProcessPipeline().SetUseDoF(use);
}

void EngineUtility::SetUseMotionBlur(bool use)
{
	_engine.GetRenderingSystem().GetPostProcessPipeline().SetUseMotionBlur(use);
}

EngineUtility::Tex2DPtr EngineUtility::LoadTextureFromFile(const std::string& path, bool hasAlpha)
{
	return _engine.GetRenderingSystem().GetTexture2DManager().LoadTextureFromFile(_engine.GetDirectX(), path, hasAlpha);
}
