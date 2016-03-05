#include "Scene.h"
#include "Director.h"

#include "EngineShaderFactory.hpp"

#include "SkyBox.h"

using namespace Core;
using namespace std;
using namespace Structure;
using namespace Math;
using namespace Rendering;
using namespace Rendering::Manager;
using namespace Rendering::PostProcessing;
using namespace Rendering::TBDR;
using namespace Rendering::Shader;
using namespace UI::Manager;
using namespace Rendering::Camera;
using namespace Rendering::Texture;
using namespace Rendering::Shadow;
using namespace Rendering::GI;
using namespace Rendering::Sky;

Scene::Scene(void) : 
	_cameraMgr(nullptr), _uiManager(nullptr),
	_renderMgr(nullptr),
	_shadowRenderer(nullptr), _globalIllumination(nullptr),
	_sky(nullptr), _ableDeallocSky(false), _backBuffer(nullptr),
	_postProcessingSystem(nullptr), _reflectionManager(nullptr)
{
	_state = State::Init;
}

Scene::~Scene(void)
{
	SAFE_DELETE(_cameraMgr);
	SAFE_DELETE(_renderMgr);
	SAFE_DELETE(_uiManager);
	SAFE_DELETE(_lightManager);	
	SAFE_DELETE(_materialMgr);
	SAFE_DELETE(_shadowRenderer);
	SAFE_DELETE(_globalIllumination);
	SAFE_DELETE(_backBuffer);
	SAFE_DELETE(_postProcessingSystem);
	SAFE_DELETE(_reflectionManager);
}

void Scene::Initialize()
{
	_dx	= Device::Director::SharedInstance()->GetDirectX();

	_cameraMgr			= new CameraManager;
	_reflectionManager	= new ReflectionProbeManager;
	_uiManager			= new UIManager;

	_renderMgr = new RenderManager;
	_renderMgr->Initialize();

	_lightManager = new LightManager;	
	_lightManager->InitializeAllShaderResourceBuffer();

	_materialMgr = new MaterialManager;
	_materialMgr->Initialize();

	_shadowRenderer = new ShadowRenderer;
	_shadowRenderer->Initialize(false);

	uint value = 0xff7fffff;
	float fltMin = (*(float*)&value);

	value = 0x7f7fffff;
	float fltMax = (*(float*)&value);

	_boundBox.SetMinMax(Vector3(fltMax, fltMax, fltMax), Vector3(fltMin, fltMin, fltMin));
	Matrix::Identity(_localMat);

	_backBuffer = new RenderTexture;
	_backBuffer->Initialize(_dx->GetBackBufferRTV(), _dx->GetBackBufferSize());

	_postProcessingSystem = new PostProcessPipeline;
	_postProcessingSystem->Initialize(_dx->GetBackBufferSize());

	NextState();
	OnInitialize();
}

void Scene::Update(float dt)
{
	OnUpdate(dt);

	auto end = _rootObjects.GetVector().end();
	for(auto iter = _rootObjects.GetVector().begin(); iter != end; ++iter)
		(*iter)->Update(dt);
}

void Scene::RenderPreview()
{
	OnRenderPreview();

	Vector3 boundBoxMin = _boundBox.GetMin();
	Vector3 boundBoxMax = _boundBox.GetMax();

	const auto& objectVector = _rootObjects.GetVector();
	for(auto iter = objectVector.begin(); iter != objectVector.end(); ++iter)
		(*iter)->UpdateTransformCB_With_ComputeSceneMinMaxPos(_dx, boundBoxMin, boundBoxMax, _localMat);

	_boundBox.SetMinMax(boundBoxMin, boundBoxMax);

	_shadowRenderer->ComputeAllLightViewProj();
	_lightManager->ComputeDirectionalLightViewProj(_boundBox, float(_shadowRenderer->GetDirectionalLightShadowMapResolution()));

	_shadowRenderer->UpdateConstBuffer(_dx);

	auto materials = _materialMgr->GetMaterials().GetVector();
	for(auto iter = materials.begin(); iter != materials.end(); ++iter)
		(*iter)->UpdateConstBuffer(_dx);

	auto FetchLightIndex = [&](const Light::LightForm* light) -> uint
	{
		return _lightManager->FetchLightIndexInEachLights(light);
	};
	auto FetchShadowIndex = [&](const Light::LightForm* light) -> uint
	{
		return _shadowRenderer->FetchShadowIndexInEachShadowLights(light);
	};

	_lightManager->UpdateSRBuffer(_dx, FetchShadowIndex);
	_shadowRenderer->UpdateSRBuffer(_dx, FetchLightIndex);

	const std::vector<CameraForm*>& cameras = _cameraMgr->GetCameraVector();
	for(auto iter = cameras.begin(); iter != cameras.end(); ++iter)
		(*iter)->CullingWithUpdateCB(_dx, _rootObjects.GetVector(), _lightManager);

	const std::vector<ReflectionProbe*>& rps = _reflectionManager->GetReflectionProbeVector();
	for(auto iter = rps.begin(); iter != rps.end(); ++iter)
		(*iter)->UpdateReflectionProbeCB(_dx, _lightManager->GetPackedLightCount());

	if(_sky)
		_sky->UpdateConstBuffer(_dx);
}

void Scene::Render()
{
	_dx->ClearDeviceContext();
	const RenderManager* renderMgr = _renderMgr;
	_shadowRenderer->RenderShadowMap(_dx, renderMgr);

	_dx->ClearDeviceContext();

	auto GIPass = [&](MeshCamera* meshCam) -> const RenderTexture*
	{
		bool isMainCam = _cameraMgr->GetMainCamera() == meshCam;
		const RenderTexture* indirectColorMap = nullptr;

		if(_globalIllumination && isMainCam)
		{
			if(meshCam->GetUseIndirectColorMap() == false)
				meshCam->ReCompileOffScreen(true);

			_globalIllumination->Run(_dx, meshCam, this);
			indirectColorMap = _globalIllumination->GetIndirectColorMap();
		}
		else
		{
			if(meshCam->GetUseIndirectColorMap())
				meshCam->ReCompileOffScreen(false);
		}

		return indirectColorMap;
	};

	auto giPassNull = std::function<const RenderTexture*(MeshCamera*)>(nullptr);

	const std::vector<CameraForm*>& cameras = _cameraMgr->GetCameraVector();
	for(auto iter = cameras.begin(); iter != cameras.end(); ++iter)
	{
		if( (*iter)->GetUsage() == CameraForm::Usage::MeshRender )
		{
			auto shadowCB = _shadowRenderer->GetShadowGlobalParamConstBuffer();
			MeshCamera* meshCam = dynamic_cast<MeshCamera*>(*iter);
			meshCam->Render(_dx, _renderMgr, _lightManager, shadowCB,
							_shadowRenderer->GetNeverUseVSM(),
							_sky,
							_globalIllumination ? GIPass : giPassNull);
		}
		else if( (*iter)->GetUsage() == CameraForm::Usage::UI )
			dynamic_cast<UICamera*>(*iter)->Render(_dx);
	}

	const auto& rps = _reflectionManager->GetReflectionProbeVector();
	for(auto iter = rps.begin(); iter != rps.end(); ++iter)
		(*iter)->Render(_dx, this);

	MeshCamera* mainCam = dynamic_cast<MeshCamera*>(_cameraMgr->GetMainCamera());
	_postProcessingSystem->Render(_dx, _backBuffer, mainCam, _sky);

	_dx->GetSwapChain()->Present(0, 0);
	OnRenderPost();
}

void Scene::Destroy()
{
	OnDestroy();
	DeleteAllObject();

	_materialMgr->DeleteAll();
	_shadowRenderer->Destroy();
	_renderMgr->Destroy();
	_uiManager->Destroy();
	_lightManager->Destroy();
	_cameraMgr->Destroy();
	_globalIllumination->Destroy();
	_postProcessingSystem->Destroy();
	_reflectionManager->Destroy();
}

void Scene::NextState()
{
	_state = (State)(((int)_state + 1) % (int)State::Num);
}

void Scene::StopState()
{
	_state = State::Stop;
}

void Scene::AddObject(Core::Object* object)
{
	_rootObjects.Add(object->GetName(), object);
}

Core::Object* Scene::FindObject(const std::string& key)
{
	Core::Object** objAddr = _rootObjects.Find(key);
	return objAddr ? (*objAddr) : nullptr;
}

void Scene::DeleteObject(Core::Object* object)
{
	std::string key = object->GetName();
	Core::Object** objectAddr = _rootObjects.Find(key);
	if(objectAddr)
	{
		Core::Object* object = (*objectAddr);
		SAFE_DELETE(object);

		_rootObjects.Delete(key);
	}
}

void Scene::DeleteAllObject()
{
	auto& objects = _rootObjects.GetVector();
	for(auto iter = objects.begin(); iter != objects.end(); ++iter)
	{
		Core::Object* object = (*iter);
		SAFE_DELETE(object);
	}

	_rootObjects.DeleteAll();
}

void Scene::Input(const Device::Win32::Mouse& mouse, const  Device::Win32::Keyboard& keyboard)
{
	if(_state == State::Loop)
		OnInput(mouse, keyboard);
}

void Scene::ActivateGI(bool activate, uint dimension, float giSize)
{
	if(activate == false)
	{
		if(_globalIllumination)
			_globalIllumination->Destroy();

		SAFE_DELETE(_globalIllumination);
	}
	else
	{
		if(_globalIllumination)
			return;

		_globalIllumination = new GlobalIllumination;
		_globalIllumination->Initialize(_dx, dimension, giSize);
	}
}

void Scene::ActiveSkyBox(const std::string& materialName, const std::string& cubeMapFilePath)
{
	if (_ableDeallocSky)
		SAFE_DELETE(_sky);

	SkyBox* skyBox = new SkyBox;
	skyBox->Initialize(materialName, cubeMapFilePath);

	_sky = skyBox;
	_ableDeallocSky = true;
}

void Scene::ActiveCustomSky(Sky::SkyForm* sky)
{
	if (_ableDeallocSky)
		SAFE_DELETE(_sky);

	_sky = sky;
	_ableDeallocSky = false;
}

void Scene::DeactivateSky()
{
	if (_ableDeallocSky)
		SAFE_DELETE(_sky);

	_sky = nullptr;
}