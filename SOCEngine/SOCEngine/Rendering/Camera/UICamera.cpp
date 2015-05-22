#include "UICamera.h"
#include "Director.h"

using namespace Rendering::Camera;
using namespace Rendering::Texture;
using namespace Rendering::Shader;
using namespace Device;
using namespace Math;

UICamera::UICamera() : Camera()
{
	_renderType = RenderType::Forward;
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

void UICamera::Render()
{
	const Device::Director* director = Device::Director::GetInstance();
	const Device::DirectX* dx = director->GetDirectX();
	ID3D11DeviceContext* context = dx->GetContext();

	_renderTarget->Clear(_clearColor, dx);

}