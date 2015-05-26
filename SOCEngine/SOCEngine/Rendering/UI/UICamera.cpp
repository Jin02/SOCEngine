#include "UICamera.h"
#include "Director.h"

using namespace Rendering::Camera;
using namespace Rendering::Texture;
using namespace Rendering::Shader;
using namespace Device;
using namespace Math;

UICamera::UICamera() : Camera(), _object(nullptr), _depthBuffer(nullptr)
{
	_renderType			= RenderType::Forward;
}

UICamera::~UICamera(void)
{
}

void UICamera::Initialize()
{
	Camera::Initialize();

	_depthBuffer =  new DepthBuffer;
	_depthBuffer->Create(Director::GetInstance()->GetWindowSize());

	_object = new Core::Object(nullptr);
	SetOwner(_object);

	_projectionType		= ProjectionType::Orthographic;
	_object->GetTransform()->UpdatePosition(Math::Vector3(0, 0, -1));

	_clearColor = Color::blue();
	_clearColor.a = 0.0f;
}

void UICamera::Destroy()
{
	SAFE_DELETE(_depthBuffer);
	Camera::Destroy();
}

void UICamera::Update(float delta)
{
	const Device::Director* director	= Device::Director::GetInstance();
	Core::Scene* scene					= director->GetCurrentScene();
	UI::Manager::UIManager* uiMgr		= scene->GetUIManager();

	auto& roots = uiMgr->GetRootUIObjects();
	for(auto iter : roots)
		iter->Update(delta);
}

void UICamera::Render()
{
	const Device::Director* director	= Device::Director::GetInstance();
	const Device::DirectX* dx			= director->GetDirectX();
	Core::Scene* scene					= director->GetCurrentScene();
	UI::Manager::UIManager* uiMgr		= scene->GetUIManager();
	ID3D11DeviceContext* context		= dx->GetContext();

	ID3D11RenderTargetView* rtv = _renderTarget->GetRenderTargetView();
	context->OMSetRenderTargets(1, &rtv, _depthBuffer->GetDepthStencilView()); 
	
	_depthBuffer->Clear(context, 1.0f, 0);
	_renderTarget->Clear(_clearColor, dx);

	ID3D11SamplerState* sampler = dx->GetLinearSamplerState();
	context->PSSetSamplers(0, 1, &sampler);

	Math::Matrix viewProjMat;
	{
		Math::Matrix viewMat;
		ViewMatrix(viewMat);

		Math::Matrix projMat;
		ProjectionMatrix(projMat);

		viewProjMat = viewMat * projMat;
	}

	auto RenderUIObj = [&](UI::UIObject* uiObj)
	{
		if(uiObj->GetUse())
		{
			uiObj->UpdateTransform(context, viewProjMat);
			uiObj->Render(context, viewProjMat);
		}
	};

	uiMgr->IterateContent(RenderUIObj);

	ID3D11SamplerState* nullSampler = nullptr;
	context->PSSetSamplers(0, 1, &nullSampler);
}