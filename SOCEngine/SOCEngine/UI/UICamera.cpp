#include "UICamera.h"
#include "Director.h"

using namespace Rendering::Camera;
using namespace Rendering::Texture;
using namespace Rendering::Shader;
using namespace Device;
using namespace Math;
using namespace Rendering::Manager;

UICamera::UICamera() : CameraForm(), _depthBuffer(nullptr)
{
}

UICamera::~UICamera(void)
{
	Destroy();
}

void UICamera::OnInitialize()
{
	CameraForm::Initialize(1);

	_depthBuffer =  new DepthBuffer;
	_depthBuffer->Initialize(Director::GetInstance()->GetBackBufferSize(), false, 1);

	_projectionType		= ProjectionType::Orthographic;
	_owner->GetTransform()->UpdatePosition(Math::Vector3(0, 0, -1));

	_clearColor = Color::Blue();
	_clearColor.a = 0.0f;
}

void UICamera::OnDestroy()
{
	SAFE_DELETE(_depthBuffer);
	CameraForm::Destroy();
}

void UICamera::Update(float delta)
{
	const Device::Director* director	= Device::Director::GetInstance();
	Core::Scene* scene					= director->GetCurrentScene();
	UI::Manager::UIManager* uiMgr		= scene->GetUIManager();

	auto& roots = uiMgr->GetRootUIObjects();
	for(const auto& iter : roots)
		iter->Update(delta);
}

void UICamera::Render(const Device::DirectX* dx, const RenderManager* renderManager, const LightManager* lightManager)
{
	const Device::Director* director	= Device::Director::GetInstance();
	Core::Scene* scene					= director->GetCurrentScene();
	UI::Manager::UIManager* uiMgr		= scene->GetUIManager();
	ID3D11DeviceContext* context		= dx->GetContext();

	ID3D11RenderTargetView* rtv = _renderTarget->GetRenderTargetView();
	context->OMSetRenderTargets(1, &rtv, _depthBuffer->GetDepthStencilView()); 
	
	_depthBuffer->Clear(context, 1.0f, 0);
	_renderTarget->Clear(context, _clearColor);

	ID3D11SamplerState* sampler = dx->GetSamplerStateLinear();
	context->PSSetSamplers(0, 1, &sampler);

	Math::Matrix viewProjMat;
	{
		Math::Matrix viewMat;
		GetViewMatrix(viewMat);

		Math::Matrix projMat;
		GetProjectionMatrix(projMat, true);

		viewProjMat = viewMat * projMat;
	}

	const std::vector<UI::UIObject*>& uiRenderQ = uiMgr->GetRenderQueue();
	for(auto iter = uiRenderQ.begin(); iter != uiRenderQ.end(); ++iter)
	{
		if( (*iter)->GetUse() )
		{
			(*iter)->UpdateTransform(context, viewProjMat);
			(*iter)->Render(context, viewProjMat);
		}
	}

	ID3D11SamplerState* nullSampler = nullptr;
	context->PSSetSamplers(0, 1, &nullSampler);
}

Core::Component* UICamera::Clone() const 
{
	UICamera* uiCam = new UICamera;
	_Clone(uiCam);

	uiCam->_depthBuffer = new DepthBuffer;
	uiCam->_depthBuffer->Initialize(Director::GetInstance()->GetBackBufferSize(), false, 1);

	return uiCam;
}