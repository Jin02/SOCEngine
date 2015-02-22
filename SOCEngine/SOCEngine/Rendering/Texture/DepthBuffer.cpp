#include "DepthBuffer.h"
#include "Director.h"

using namespace Rendering;
using namespace Rendering::Texture;

DepthBuffer::DepthBuffer(const Device::DirectX* dx) : _depthStencilView(nullptr), RenderTexture(),
	_dx(dx)
{

}

DepthBuffer::~DepthBuffer()
{
	Destroy();
}

bool DepthBuffer::Create(const Math::Size<unsigned int>& size, bool useShaderResource)
{
	unsigned int bindFlag =	D3D11_BIND_DEPTH_STENCIL | 
							(useShaderResource ? D3D11_BIND_SHADER_RESOURCE : 0);
	if(_Create(size, DXGI_FORMAT_D32_FLOAT, bindFlag) == false)
	{
		ASSERT_MSG("Error");
		return false;
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC desc;
	memset(&desc, 0, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	desc.Format = DXGI_FORMAT_D32_FLOAT;
	{
		D3D11_TEXTURE2D_DESC texDesc;
		_texture->GetDesc(&texDesc);
		desc.ViewDimension = texDesc.SampleDesc.Count > 1 ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
	}
	desc.Texture2D.MipSlice = 0;

	ID3D11Device* device = Device::Director::GetInstance()->GetDirectX()->GetDevice();
	HRESULT hr = device->CreateDepthStencilView(_texture, &desc, &_depthStencilView); 
	if(FAILED(hr))
	{
		ASSERT_MSG("Error");
		return false;
	}

	return true;
}

void DepthBuffer::Destroy()
{
	RenderTexture::Destroy();
	SAFE_RELEASE(_depthStencilView);
}

void DepthBuffer::SetRenderTarget(const Device::DirectX* dx)
{
	//null
}

void DepthBuffer::Clear(float depth, unsigned char stencil)
{
	_dx->GetContext()->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH, depth, stencil);
}