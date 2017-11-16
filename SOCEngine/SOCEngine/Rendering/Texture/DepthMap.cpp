#include "DepthMap.h"
#include "DirectX.h"

using namespace Rendering;
using namespace Rendering::Texture;

void DepthMap::Initialize(Device::DirectX& dx,
	const Size<uint>& size, bool useShaderResource, uint sampleCount)
{
	unsigned int bindFlag =	D3D11_BIND_DEPTH_STENCIL | 
							(useShaderResource ? D3D11_BIND_SHADER_RESOURCE : 0);

	_tex2D.Initialize(dx, size.w, size.h, DXGI_FORMAT_D32_FLOAT, DXGI_FORMAT_D32_FLOAT, bindFlag, sampleCount, 1);

	D3D11_DEPTH_STENCIL_VIEW_DESC desc;
	memset(&desc, 0, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	desc.Format = DXGI_FORMAT_D32_FLOAT;
	{
		D3D11_TEXTURE2D_DESC texDesc;
		_tex2D.GetTexture().GetRaw()->GetDesc(&texDesc);
		desc.ViewDimension = texDesc.SampleDesc.Count > 1 ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
	}
	desc.Texture2D.MipSlice = 0;

	_depthStencilView = dx.CreateDepthStencilView(_tex2D.GetTexture().GetRaw(), desc);
}

void DepthMap::Destroy()
{
	_depthStencilView.Destroy();
	_tex2D.Destroy();
}

void DepthMap::Clear(Device::DirectX& dx, float depth, unsigned char stencil)
{
	dx.GetContext()->ClearDepthStencilView(_depthStencilView.GetRaw(), D3D11_CLEAR_DEPTH, depth, stencil);
}
