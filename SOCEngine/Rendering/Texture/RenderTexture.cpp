#include "RenderTexture.h"
#include "DirectX.h"
#include "Common.h"

using namespace Rendering::Texture;
using namespace Device;

RenderTexture::RenderTexture() :_texture(nullptr), _renderTargetView(nullptr), _shaderResourceView(nullptr)
{

}

RenderTexture::~RenderTexture()
{
}

bool RenderTexture::Create(const DirectX* dx, int width, int height)
{
	ID3D11Device* device = dx->GetDevice();

	D3D11_TEXTURE2D_DESC textureDesc;
	memset(&textureDesc, 0, sizeof(D3D11_TEXTURE2D_DESC));

	// Setup the render target texture description.
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	if(FAILED(device->CreateTexture2D(&textureDesc, NULL, &_texture)))
		return false;

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	if(FAILED(device->CreateRenderTargetView(_texture, &renderTargetViewDesc, &_renderTargetView)))
		return false;

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

	// Setup the description of the shader resource view.
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	// Create the shader resource view.
	if(FAILED(device->CreateShaderResourceView(_texture, &shaderResourceViewDesc, &_shaderResourceView)))
		return false;

	return true;
}

void RenderTexture::Destroy()
{
	SAFE_RELEASE(_texture);
	SAFE_RELEASE(_renderTargetView);
	SAFE_RELEASE(_shaderResourceView);
}

void RenderTexture::SetRenderTarget(const DirectX* dx)
{		
	dx->GetContext()->OMSetRenderTargets(1, &_renderTargetView, dx->GetDepthStencilView());
}

void RenderTexture::Clear(const DirectX* dx, const Rendering::Color& color)
{
	ID3D11DeviceContext* context = dx->GetContext();

	// Clear the back buffer.
	const float colorValue[] = {color.r, color.g, color.b, color.a};
	context->ClearRenderTargetView(_renderTargetView, colorValue);

	// Clear the depth buffer.
	context->ClearDepthStencilView(dx->GetDepthStencilView(), D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
}