#include "RenderTexture.h"
#include "Director.h"

using namespace Rendering::Texture;
using namespace Device;

RenderTexture::RenderTexture() : Texture(), _renderTargetView(nullptr)
{

}

RenderTexture::~RenderTexture()
{
	Destroy();
}

bool RenderTexture::Initialize(const Math::Size<unsigned int>& size, DXGI_FORMAT format)
{
	const uint bindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	
	Texture::Initialize(size, format, bindFlags);

	if(bindFlags & D3D11_BIND_RENDER_TARGET)
	{
		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
		renderTargetViewDesc.Format = format;
		renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		renderTargetViewDesc.Texture2D.MipSlice = 0;

		const Device::DirectX* dx = Device::Director::GetInstance()->GetDirectX();
		ID3D11Device* device = dx->GetDevice();

		if(FAILED(device->CreateRenderTargetView(_texture, &renderTargetViewDesc, &_renderTargetView)))
		{
			ASSERT_MSG("Error, not create render targer view, check srv desc or texture desc");
			return false;
		}
	}

	return true;
}

void RenderTexture::Destroy()
{
	SAFE_RELEASE(_texture);
	SAFE_RELEASE(_renderTargetView);
}

void RenderTexture::SetRenderTarget(ID3D11DeviceContext* context, const DepthBuffer* depthBuffer)
{
	context->OMSetRenderTargets(1, &_renderTargetView, depthBuffer->GetDepthStencilView());
}

void RenderTexture::Clear(ID3D11DeviceContext* context, const Rendering::Color& color)
{
	// clear the back buffer.
	const float colorValue[] = {color.r, color.g, color.b, color.a};
	context->ClearRenderTargetView(_renderTargetView, colorValue);
}