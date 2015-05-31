#include "ForwardPlusCamera.h"
#include "Director.h"

using namespace Rendering::Camera;
using namespace Rendering::Texture;
using namespace Rendering::Shader;
using namespace Device;
using namespace Math;

ForwardPlusCamera::ForwardPlusCamera()
	: Camera(), _opaqueDepthBuffer(nullptr), _transparentDepthBuffer(nullptr), _useTransparentRender(false)
{
	_renderType = RenderType::ForwardPlus;
}

ForwardPlusCamera::~ForwardPlusCamera()
{
}

void ForwardPlusCamera::Initialize()
{
	Camera::Initialize();
	const Size<unsigned int> windowSize = Director::GetInstance()->GetWindowSize();

	_opaqueDepthBuffer =  new DepthBuffer;
	_opaqueDepthBuffer->Create(windowSize);

	_transparentDepthBuffer =  new DepthBuffer;
	_transparentDepthBuffer->Create(windowSize);
}

void ForwardPlusCamera::Destroy()
{
	SAFE_DELETE(_opaqueDepthBuffer);
	SAFE_DELETE(_transparentDepthBuffer);

	Camera::Destroy();
}

void ForwardPlusCamera::Render()
{
	const Device::Director* director = Device::Director::GetInstance();
	const Device::DirectX* dx = director->GetDirectX();
	ID3D11DeviceContext* context = dx->GetContext();

	const Manager::RenderManager* renderMgr = director->GetCurrentScene()->GetRenderManager();


	_renderTarget->Clear(_clearColor, dx);
	_opaqueDepthBuffer->Clear(context, 0.0f, 0); //inverted depth를 사용함. 그러므로 0으로 초기화

	if(_useTransparentRender)
		_transparentDepthBuffer->Clear(context, 0.0f, 0); //inverted depth

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
			context->OMSetRenderTargets(1, &nullRenderTargetView, _opaqueDepthBuffer->GetDepthStencilView() );
			context->OMSetDepthStencilState( dx->GetDepthGreaterState(), 0 ); //inverted 32bit depth 버퍼를 씀. inverted로 하는게 더 정밀도가 높음
		}
	
		// alpha test
		{
		}

		// restore
		{
			context->RSSetState(nullptr);
			context->OMSetBlendState(dx->GetOpaqueBlendState(), blendFactor, 0xffffffff);
		}
	}
}