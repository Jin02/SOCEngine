#include "ScreenSpaceRayTracing.h"
#include "Director.h"
#include "BindIndexInfo.h"
#include "ResourceManager.h"
#include "SkyBox.h"

using namespace Rendering;
using namespace Rendering::PostProcessing;
using namespace Rendering::Texture;
using namespace Rendering::Shader;
using namespace Rendering::Buffer;
using namespace Rendering::Camera;
using namespace Rendering::TBDR;
using namespace Rendering::Sky;
using namespace Device;
using namespace Math;
using namespace Resource;

ScreenSpaceRayTracing::ScreenSpaceRayTracing() :
	_rayTracingBuffer(nullptr), _viewToTexSpaceCB(nullptr)
{
}

ScreenSpaceRayTracing::~ScreenSpaceRayTracing()
{
	SAFE_DELETE(_rayTracingBuffer);
	SAFE_DELETE(_viewToTexSpaceCB);
}

void ScreenSpaceRayTracing::Initialize(	const Device::DirectX* dx, const Buffer::ConstBuffer* ssrtParam )
{
	std::vector<ShaderMacro> macros;
	macros.push_back(Director::SharedInstance()->GetDirectX()->GetMSAAShaderMacro());

	FullScreen::Initialize("ScreenSpaceRayTracing", "SSRT_InFullScreen_PS", &macros);

	_viewToTexSpaceCB = new ConstBuffer;
	_viewToTexSpaceCB->Initialize(sizeof(Matrix));

//	_inputConstBuffers.push_back(ShaderForm::InputConstBuffer(0, tbrParam));
	_inputConstBuffers.push_back(ShaderForm::InputConstBuffer(5, _viewToTexSpaceCB));
	_inputConstBuffers.push_back(ShaderForm::InputConstBuffer(6, ssrtParam));

	const auto& size = dx->GetBackBufferSize();

	_rayTracingBuffer = new RenderTexture;
	_rayTracingBuffer->Initialize(size, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_UNKNOWN, 0, 1);
}

void ScreenSpaceRayTracing::UpdateConstBuffer(const Device::DirectX* dx)
{
}

void ScreenSpaceRayTracing::Render(const DirectX* dx, const GBuffers& gbuffer)
{
	ID3D11DeviceContext* context	= dx->GetContext();

	// GBuffer Setting
	{
	}
	
	FullScreen::Render(dx, _rayTracingBuffer);
}

void ScreenSpaceRayTracing::Destroy()
{
	_rayTracingBuffer->Destroy();
	_viewToTexSpaceCB->Destory();
}
