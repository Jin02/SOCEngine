#include "RenderTexture.h"
#include "Director.h"

using namespace Rendering::Texture;
using namespace Device;

RenderTexture::RenderTexture()
	: Texture2D(nullptr, nullptr, false), _renderTargetView(nullptr)
{

}

RenderTexture::~RenderTexture()
{
	Destroy();
}

bool RenderTexture::Initialize(const Math::Size<unsigned int>& size, DXGI_FORMAT srvFormat, DXGI_FORMAT rtvFormat, DXGI_FORMAT uavFormat, uint optionalBindFlags, uint sampleCount)
{
	const uint bindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | ((uavFormat != DXGI_FORMAT_UNKNOWN) ? D3D11_BIND_UNORDERED_ACCESS : 0) | optionalBindFlags;
	
	Texture2D::Initialize(size.w, size.h, srvFormat, uavFormat, bindFlags, sampleCount, 1);

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	memset(&renderTargetViewDesc, 0, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));

	renderTargetViewDesc.Format = rtvFormat;

	D3D11_TEXTURE2D_DESC texDesc;
	_texture->GetDesc(&texDesc);

	renderTargetViewDesc.ViewDimension = (texDesc.SampleDesc.Count > 1) ? D3D11_RTV_DIMENSION_TEXTURE2DMS : D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	ID3D11Device* device = Device::Director::SharedInstance()->GetDirectX()->GetDevice();

	if(FAILED(device->CreateRenderTargetView(_texture, &renderTargetViewDesc, &_renderTargetView)))
	{
		ASSERT_MSG("Error, not create render targer view, check srv desc or texture desc");
		return false;
	}

	return true;
}

bool RenderTexture::Initialize(ID3D11RenderTargetView* rtv, const Math::Size<uint>& size)
{
	ASSERT_MSG_IF(rtv, "Error, rtv is null");
	{
		_renderTargetView	= rtv;
		_size				= size;
	}
	
	return true;
}

void RenderTexture::Destroy()
{
	SAFE_RELEASE(_texture);
	SAFE_RELEASE(_renderTargetView);
}
//
//void RenderTexture::SetRenderTarget(ID3D11DeviceContext* context, const DepthBuffer* depthBuffer)
//{
//	context->OMSetRenderTargets(1, &_renderTargetView, depthBuffer->GetDepthStencilView());
//}

void RenderTexture::Clear(ID3D11DeviceContext* context, const Rendering::Color& color)
{
	// clear the back buffer.
	const float colorValue[] = {color.r, color.g, color.b, color.a};
	context->ClearRenderTargetView(_renderTargetView, colorValue);
}