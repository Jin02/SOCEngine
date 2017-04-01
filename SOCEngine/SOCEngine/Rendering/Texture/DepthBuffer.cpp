#include "DepthBuffer.h"

using namespace Rendering;
using namespace Rendering::Texture;

DepthBuffer::DepthBuffer() 
	: Texture2D(), _depthStencilView()
{
}

DepthBuffer::~DepthBuffer()
{
	Destroy();
}

void DepthBuffer::Initialize(Device::DirectX& dx,
	const Size<uint>& size, bool useShaderResource, uint sampleCount)
{
	unsigned int bindFlag =	D3D11_BIND_DEPTH_STENCIL | 
							(useShaderResource ? D3D11_BIND_SHADER_RESOURCE : 0);

	Texture2D::Initialize(dx, size.w, size.h, DXGI_FORMAT_D32_FLOAT, DXGI_FORMAT_D32_FLOAT, bindFlag, sampleCount, 1);

	D3D11_DEPTH_STENCIL_VIEW_DESC desc;
	memset(&desc, 0, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	desc.Format = DXGI_FORMAT_D32_FLOAT;
	{
		D3D11_TEXTURE2D_DESC texDesc;
		GetTexture().GetRaw()->GetDesc(&texDesc);
		desc.ViewDimension = texDesc.SampleDesc.Count > 1 ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
	}
	desc.Texture2D.MipSlice = 0;

	_depthStencilView = dx.CreateDepthStencilView(GetTexture().GetRaw(), desc);
}

void DepthBuffer::Destroy()
{
	_depthStencilView.Destroy();
}

void DepthBuffer::Clear(Device::DirectX& dx, float depth, unsigned char stencil)
{
	dx.GetContext()->ClearDepthStencilView(_depthStencilView.GetRaw(), D3D11_CLEAR_DEPTH, depth, stencil);
}