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

	const Size<unsigned int> windowSize = Director::GetInstance()->GetWindowSize();
	_depthBuffer->Create(windowSize);

	_object = new Core::Object(nullptr);
	SetOwner(_object);

	_projectionType		= ProjectionType::Orthographic;
	_object->GetTransform()->UpdatePosition(Math::Vector3(0, 0, -1));
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

	ID3D11DepthStencilState* originDepthStencilState = nullptr;
	context->OMGetDepthStencilState(&originDepthStencilState, 0);
	
	//turn off depth writing
	context->OMSetDepthStencilState(dx->GetDepthDisableDepthTestState(), 0);

	ID3D11RenderTargetView* rtv = dx->GetBackBuffer();//_renderTarget->GetRenderTargetView();
	context->OMSetRenderTargets(1, &rtv, _depthBuffer->GetDepthStencilView()); 
	
	_depthBuffer->Clear(dx, 1.0f, 0);

	float clearColor[] = {_clearColor.r, _clearColor.g, _clearColor.b, 1.0f};
	context->ClearRenderTargetView(rtv, clearColor);
	//_renderTarget->Clear(_clearColor, dx);

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
		uiObj->UpdateTransform(context, viewProjMat);
		uiObj->Render(viewProjMat);
	};

	uiMgr->IterateContent(RenderUIObj);

	//restore depth stencil state
	context->OMSetDepthStencilState(originDepthStencilState, 0);
}