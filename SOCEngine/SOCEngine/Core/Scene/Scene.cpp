#include "Scene.h"
#include "Director.h"

#include "EngineShaderFactory.hpp"
#include "FontLoader.h"

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

Scene::Scene(void) : 
	_cameraMgr(nullptr), _uiManager(nullptr),
	_renderMgr(nullptr), _backBufferMaker(nullptr),
	_shadowRenderer(nullptr)
{
	_state = State::Init;
}

Scene::~Scene(void)
{
	Destroy();
}

void Scene::Initialize()
{
	_cameraMgr		= new CameraManager;

	_renderMgr		= new RenderManager;
	_renderMgr->Initialize();

	_uiManager		= new UIManager;

	_dx				= Device::Director::GetInstance()->GetDirectX();

	_lightManager	= new LightManager;	
	_lightManager->InitializeAllShaderResourceBuffer();

	_materialMgr	= new MaterialManager;

	_backBufferMaker = new BackBufferMaker;
	_backBufferMaker->Initialize(false);

	_shadowRenderer = new ShadowRenderer;
	_shadowRenderer->Initialize(true);

	uint value = 0xff7fffff;
	float fltMin = (*(float*)&value);

	value = 0x7f7fffff;
	float fltMax = (*(float*)&value);

	_boundBox.SetMinMax(Vector3(fltMax, fltMax, fltMax), Vector3(fltMin, fltMin, fltMin));

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
		(*iter)->UpdateTransformCB_With_ComputeSceneMinMaxPos(_dx, boundBoxMin, boundBoxMax);

	_boundBox.SetMinMax(boundBoxMin, boundBoxMax);
	_shadowRenderer->UpdateConstBuffer(_dx);

	auto materials = _materialMgr->GetMaterials().GetVector();
	for(auto iter = materials.begin(); iter != materials.end(); ++iter)
		(*iter)->UpdateConstBuffer(_dx);

	_lightManager->ComputeAllLightViewProj(_boundBox);
	_lightManager->UpdateBuffer(_dx);

	const std::vector<CameraForm*>& cameras = _cameraMgr->GetVector();
	for(auto iter = cameras.begin(); iter != cameras.end(); ++iter)
		(*iter)->CullingWithUpdateCB(_dx, _rootObjects.GetVector(), _lightManager);
}

void Scene::Render()
{
	_dx->ClearDeviceContext();
	const RenderManager* renderMgr = _renderMgr;
	_shadowRenderer->RenderShadowMap(_dx, renderMgr);

	_dx->ClearDeviceContext();
	const std::vector<CameraForm*>& cameras = _cameraMgr->GetVector();
	for(auto iter = cameras.begin(); iter != cameras.end(); ++iter)
	{
		if( (*iter)->GetUsage() == CameraForm::Usage::MeshRender )
		{
			const Buffer::ConstBuffer* shadowCB = _shadowRenderer->IsWorking() ? _shadowRenderer->GetShadowGlobalParamConstBuffer() : nullptr;
			dynamic_cast<MeshCamera*>(*iter)->Render(_dx, _renderMgr, _lightManager, shadowCB);
		}
		else if( (*iter)->GetUsage() == CameraForm::Usage::UI )
			dynamic_cast<UICamera*>(*iter)->Render(_dx);
	}
	CameraForm* firstCam = _cameraMgr->GetFirstCamera();
	if(firstCam)
	{
		ID3D11RenderTargetView* backBufferRTV = _dx->GetBackBufferRTV();
		const RenderTexture* camRT = firstCam->GetRenderTarget();

		MeshCamera* mainFirstCam = dynamic_cast<MeshCamera*>(firstCam);
		_backBufferMaker->Render(backBufferRTV, camRT, nullptr, mainFirstCam->GetTBRParamConstBuffer());
	}

	_dx->GetSwapChain()->Present(0, 0);
	OnRenderPost();
}

void Scene::Destroy()
{
	UI::FontLoader::GetInstance()->Destroy();

	SAFE_DELETE(_cameraMgr);
	SAFE_DELETE(_renderMgr);
	SAFE_DELETE(_uiManager);
	SAFE_DELETE(_lightManager);	
	SAFE_DELETE(_materialMgr);
	SAFE_DELETE(_backBufferMaker);
	SAFE_DELETE(_shadowRenderer);

	OnDestroy();
}

void Scene::NextState()
{
	_state = (State)(((int)_state + 1) % (int)State::Num);
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