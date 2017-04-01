#include "RenderTexture.h"
#include <assert.h>

using namespace Rendering::Texture;
using namespace Device;

void RenderTexture::Initialize(
	Device::DirectX& dx,
	const Size<uint>& size,
	DXGI_FORMAT srvFormat, DXGI_FORMAT rtvFormat, DXGI_FORMAT uavFormat,
	uint optionalBindFlags, uint sampleCount, uint mipLevel)
{
	const uint bindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | ((uavFormat != DXGI_FORMAT_UNKNOWN) ? D3D11_BIND_UNORDERED_ACCESS : 0) | optionalBindFlags;
	
	_tex2D.Initialize(dx, size.w, size.h, srvFormat, uavFormat, bindFlags, sampleCount, mipLevel);

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	memset(&renderTargetViewDesc, 0, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));

	renderTargetViewDesc.Format = rtvFormat;

	D3D11_TEXTURE2D_DESC texDesc;
	_tex2D.GetTexture().GetRaw()->GetDesc(&texDesc);

	renderTargetViewDesc.ViewDimension = (texDesc.SampleDesc.Count > 1) ? D3D11_RTV_DIMENSION_TEXTURE2DMS : D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	_renderTargetView = dx.CreateRenderTargetView(_tex2D.GetTexture().GetRaw(), renderTargetViewDesc);
}

void RenderTexture::Initialize(Device::DirectX& dx,
	const DXResource<ID3D11RenderTargetView>& rtv, const Size<uint>& size)
{
	ASSERT_SUCCEEDED(rtv.IsCanUse());

	_renderTargetView	= rtv;
	_tex2D.SetSize(size);
}

void RenderTexture::Clear(Device::DirectX& dx, const Color& color)
{
	// clear the back buffer.
	const float colorValue[] = {color.r, color.g, color.b, color.a};
	dx.GetContext()->ClearRenderTargetView(_renderTargetView.GetRaw(), colorValue);
}
