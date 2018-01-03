#pragma once

#include "Common.h"
#include "IScene.h"
#include <memory>
#include <chrono>
#include "ComponentSystem.h"
#include "ObjectManager.h"
#include "RootObjectIDs.hpp"

#include "Singleton.h"
#include "Rect.h"

#include "RenderingSystem.h"
#include "MeshImporter.h"

namespace Core
{
	class Engine;

	class EngineUtility final
	{
	public:
		EngineUtility(Engine& e);

		// Mesh
		void OpaqueMeshToTransparentMesh(ObjectID meshObjID);
		void OpaqueMeshToAlphaTestMesh(ObjectID meshObjID);

		void AlphaTestMeshToOpaqueMesh(ObjectID meshObjID);
		void AlphaTestMeshToTransparentMesh(ObjectID meshObjID);

		void TransparentMeshToOpaqueMesh(ObjectID meshObjID);
		void TransparentMeshToAlphaTestMesh(ObjectID meshObjID);

		// Material
		Rendering::Material::PhysicallyBasedMaterial AcquireMaterial(const std::string& name);
		Rendering::MaterialID AddMaterialToPool(Rendering::Material::PhysicallyBasedMaterial& pbm);

		// VXGI
		void SetVXGICenterPosition(const Math::Vector3& worldPos);

		// Object
		Object& AcquireObject(const std::string& name);
		Object* FindObject(const std::string& name);

		// Importer
		Object* LoadMesh(const std::string& fileDir, bool useDynamicVB = false, bool useDynamicIB = false);

		// SkyBox
		void SetSkyBoxToMainCamera(Rendering::MaterialID skyBox);
		void ActivateSkyBox(const std::string& cubeTexturePath, const std::string& materialKey);

		// SkyScattering
		void ActivateSkyScattering(uint resolution, const Object& directionalLight);
		void DeactivateSkyScattering();

		// PostProcessing
		void SetSunShaftParam(ObjectID directionalLightID, float circleSize, float circleIntensity);
		void SetUseDoF(bool use);
		void SetUseSSAO(bool use);
		void SetUseSunShaft(bool use);

		// Texture
		using Tex2DPtr = Rendering::Manager::Texture2DManager::Texture2DPtr;
		Tex2DPtr LoadTextureFromFile(const std::string& path, bool hasAlpha);

	private:
		Engine& _engine;
	};
}