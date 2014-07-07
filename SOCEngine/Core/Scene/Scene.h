#pragma once

#include "Rendering.h"
#include "Container.h"
#include "BaseScene.h"


class Scene : public BaseScene
{
public:
	bool destroyMgr;

protected:
	Container<Rendering::Object> *rootObjects;

protected:
	Device::Graphics			*graphics;
	Rendering::Light::LightManager			*lightMgr;
	Rendering::Texture::TextureManager		*textureMgr;
	Rendering::Shader::ShaderManager		*shaderMgr;
	Rendering::CameraManager				*cameraMgr;
	Rendering::Material::MaterialManager	*materialMgr;
	//Rendering::Mesh::VBElementsManager		*vbElementsMgr;
	Rendering::MeshDataManager				*meshDataMgr;

public:
	Scene(void);
	~Scene(void);

private:
	virtual void Initialize();
	virtual void Update(float dt);
	virtual void Render();
	virtual void Destroy();

public:
	virtual void OnInitialize() = 0;
	virtual void OnRenderPreview() = 0;
	virtual void OnUpdate(float dt) = 0;
	virtual void OnRenderPost() = 0;
	virtual void OnDestroy() = 0;

public:
	Rendering::Light::LightManager* GetLightManager();
	Rendering::Texture::TextureManager* GetTextureManager();
	Rendering::Shader::ShaderManager* GetShaderManager();
	Rendering::CameraManager* GetCameraManager();
	Rendering::MeshDataManager* GetMeshDataMgr();
	Rendering::Material::MaterialManager* GetMaterialMgr();
};