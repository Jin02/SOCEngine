#include "UICamera.h"
#include "Director.h"
#include "BindIndexInfo.h"

using namespace Rendering;
using namespace Rendering::Camera;
using namespace Rendering::Texture;
using namespace Rendering::Shader;
using namespace Device;
using namespace Math;
using namespace Rendering::Manager;

UICamera::UICamera() : CameraForm(Usage::UI), _depthBuffer(nullptr)
{
}

UICamera::~UICamera(void)
{
	Destroy();
}

void UICamera::OnInitialize()
{
	Size<float> backBufferSize = Director::SharedInstance()->GetBackBufferSize().Cast<float>();
	CameraForm::Initialize(Math::Rect<float>(0.0f, 0.0f, backBufferSize.w, backBufferSize.h), false);

	_depthBuffer =  new DepthBuffer;
	_depthBuffer->Initialize(Director::SharedInstance()->GetBackBufferSize(), false, 1);

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
	const Device::Director* director	= Device::Director::SharedInstance();
	Core::Scene* scene					= director->GetCurrentScene();
	UI::Manager::UIManager* uiMgr		= scene->GetUIManager();

	auto& roots = uiMgr->GetRootUIObjects();
	for(const auto& iter : roots)
		iter->Update(delta);
}

void UICamera::Render(const Device::DirectX* dx)
{
	const Device::Director* director	= Device::Director::SharedInstance();
	Core::Scene* scene					= director->GetCurrentScene();
	UI::Manager::UIManager* uiMgr		= scene->GetUIManager();
	ID3D11DeviceContext* context		= dx->GetContext();

	ID3D11RenderTargetView* rtv = _renderTarget->GetRenderTargetView();
	context->OMSetRenderTargets(1, &rtv, _depthBuffer->GetDepthStencilView()); 
	
	_depthBuffer->Clear(context, 1.0f, 0);
	_renderTarget->Clear(context, _clearColor);

	PixelShader::BindSamplerState(context, SamplerStateBindIndex::DefaultSamplerState, dx->GetSamplerStateLinear());

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

	PixelShader::BindSamplerState(context, SamplerStateBindIndex::DefaultSamplerState, nullptr);
}

void UICamera::CullingWithUpdateCB(const Device::DirectX* dx, const std::vector<Core::Object*>& objects, const Manager::LightManager* lightManager, bool useHDR)
{
	ASSERT_MSG("Not use");
}

Core::Component* UICamera::Clone() const 
{
	UICamera* uiCam = new UICamera;
	_Clone(uiCam);

	uiCam->_depthBuffer = new DepthBuffer;
	uiCam->_depthBuffer->Initialize(Director::SharedInstance()->GetBackBufferSize(), false, 1);

	return uiCam;
}
