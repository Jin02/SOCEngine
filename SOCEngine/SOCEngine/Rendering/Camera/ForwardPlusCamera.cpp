#include "ForwardPlusCamera.h"
#include "Director.h"

using namespace Rendering::Camera;
using namespace Rendering::Texture;
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

void ForwardPlusCamera::Clear(const Device::DirectX* dx)
{
	ID3D11DeviceContext* context = dx->GetContext();

	_renderTarget->Clear(_clearColor, dx);
	_opaqueDepthBuffer->Clear(dx, 0.0f, 0); //inverted depth를 사용함. 그러므로 0으로 초기화

	if(_useTransparentRender)
		_transparentDepthBuffer->Clear(dx, 0.0f, 0); //inverted depth

	ID3D11RenderTargetView*		nullRenderTargetView	= nullptr;
	ID3D11DepthStencilView*		nullDepthStencilView	= nullptr;
	ID3D11ShaderResourceView*	nullShaderResourceView	= nullptr;
	ID3D11UnorderedAccessView*	nullUnorderedAccessView	= nullptr;
	ID3D11SamplerState*			nullSamplerState		= nullptr;

	// 1 - Depth Prepass
	{
		context->OMSetRenderTargets(1, &nullRenderTargetView, _opaqueDepthBuffer->GetDepthStencilView() );
		context->OMSetDepthStencilState( dx->GetDepthDisableDepthTestState(), 0 ); //inverted 32bit depth 버퍼를 씀. inverted로 하는게 더 정밀도가 높음
		context->VSSetShader(nullptr, nullptr, 0);
		context->PSSetShader(nullptr, nullptr, 0);

		context->PSSetShaderResources(0, 1, &nullShaderResourceView); //clear transform
		context->PSSetShaderResources(0, 1, &nullShaderResourceView); //

		context->PSSetSamplers(0, 1, &nullSamplerState);
	}
}