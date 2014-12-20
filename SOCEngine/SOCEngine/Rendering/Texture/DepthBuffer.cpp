#include "DepthBuffer.h"
#include "Director.h"

using namespace Rendering;
using namespace Rendering::Texture;

DepthBuffer::DepthBuffer() : _depthStencilView(nullptr), RenderTexture()
{

}

DepthBuffer::~DepthBuffer()
{
	Destroy();
}

bool DepthBuffer::Create(const Math::Size<unsigned int>& size)
{
	return _Create(size, DXGI_FORMAT_R32_FLOAT, D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_RENDER_TARGET, true);
}

void DepthBuffer::Destroy()
{
	SAFE_RELEASE(_texture);
	SAFE_RELEASE(_depthStencilView);
	SAFE_RELEASE(_shaderResourceView);
}

void DepthBuffer::SetRenderTarget(const Device::DirectX* dx)
{
	if(dx == nullptr)
		dx = Device::Director::GetInstance()->GetDirectX();

	dx->GetContext()->OMSetRenderTargets(1, &_renderTargetView, _depthStencilView);
}

void DepthBuffer::Clear(float depth, unsigned char stencil, const Device::DirectX* dx)
{
	if(dx == nullptr)
		dx = Device::Director::GetInstance()->GetDirectX();

	dx->GetContext()->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH, depth, stencil);
}