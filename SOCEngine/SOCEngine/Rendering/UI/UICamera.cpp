#include "UICamera.h"
#include "Director.h"

using namespace Rendering::Camera;
using namespace Rendering::Texture;
using namespace Rendering::Shader;
using namespace Device;
using namespace Math;

UICamera::UICamera() : Camera()
{
	_renderType			= RenderType::Forward;
	_projectionType		= ProjectionType::Orthographic;
}

UICamera::~UICamera(void)
{
}

void UICamera::Initialize()
{
	Camera::Initialize();
}

void UICamera::Destroy()
{
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

	_renderTarget->Clear(_clearColor, dx);

	Math::Matrix viewProjMat;
	{
		Math::Matrix viewMat;
		ViewMatrix(viewMat);

		Math::Matrix projMat;
		ProjectionMatrix(projMat);

		viewProjMat = viewMat * projMat;
	}

	ID3D11DeviceContext* context		= dx->GetContext();
	auto RenderUIObj = [&](UI::UIObject* uiObj)
	{
		uiObj->Render(viewProjMat);
	};

	uiMgr->IterateContent(RenderUIObj);
}