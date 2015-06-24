#include "ForwardPlusCamera.h"
#include "Director.h"

using namespace Rendering::Camera;
using namespace Rendering::Texture;
using namespace Rendering::Shader;
using namespace Device;
using namespace Math;
using namespace Rendering::Light;

ForwardPlusCamera::ForwardPlusCamera() : Camera(),
	_opaqueDepthBuffer(nullptr), _blendedDepthBuffer(nullptr), _lightCulling(nullptr)
{
	_renderType = RenderType::ForwardPlus;
}

ForwardPlusCamera::~ForwardPlusCamera()
{
}

void ForwardPlusCamera::OnInitialize()
{
	Camera::OnInitialize();
	const Size<unsigned int> windowSize = Director::GetInstance()->GetWindowSize();

	_opaqueDepthBuffer =  new DepthBuffer;
	_opaqueDepthBuffer->Initialize(windowSize);

	EnableRenderBlendedMesh(true);
}

void ForwardPlusCamera::OnDestroy()
{
	SAFE_DELETE(_opaqueDepthBuffer);
	SAFE_DELETE(_blendedDepthBuffer);
	SAFE_DELETE(_lightCulling);

	Camera::OnDestroy();
}

void ForwardPlusCamera::Render()
{
	const Device::Director* director = Device::Director::GetInstance();
	const Device::DirectX* dx = director->GetDirectX();
	ID3D11DeviceContext* context = dx->GetContext();

	const Manager::RenderManager* renderMgr = director->GetCurrentScene()->GetRenderManager();

	_renderTarget->Clear(context, _clearColor);
	_opaqueDepthBuffer->Clear(context, 0.0f, 0); //inverted depth를 사용함. 그러므로 0으로 초기화
	_blendedDepthBuffer->Clear(context, 0.0f, 0); //inverted depth

	ID3D11RenderTargetView*		nullRenderTargetView	= nullptr;
	ID3D11DepthStencilView*		nullDepthStencilView	= nullptr;
	ID3D11ShaderResourceView*	nullShaderResourceView	= nullptr;
	ID3D11UnorderedAccessView*	nullUnorderedAccessView	= nullptr;
	ID3D11SamplerState*			nullSamplerState		= nullptr;

	float blendFactor[] = {0, 0, 0, 0};

	// 1 - Depth Prepass
	{
		// opaque
		{
		}
	
		// alpha test
		{
		}

		// restore
		{
		}
	}
}

void ForwardPlusCamera::EnableRenderBlendedMesh(bool enable)
{
	const Size<unsigned int> windowSize = Director::GetInstance()->GetWindowSize();
	
	SAFE_DELETE(_blendedDepthBuffer);
	if(enable)
	{
		_blendedDepthBuffer =  new DepthBuffer;
		_blendedDepthBuffer->Initialize(windowSize);
	}

	SAFE_DELETE(_lightCulling);
	_lightCulling = new LightCulling;
	_lightCulling->Initialize(TEMP_LIGHT_CULLING_HLSL_PATH, enable);
}