#include "Texture2D.h"
#include <assert.h>

using namespace Rendering::Texture;
using namespace Rendering::View;

Texture2D::Texture2D(const ShaderResourceView& srv, const DXResource<ID3D11Texture2D>& tex, bool hasAlpha, const Size<uint>& size)
	: _base(srv), _texture(tex), _size(size)
{
}

void Texture2D::Initialize(Device::DirectX& dx,
	uint width, uint height,
	DXGI_FORMAT srvFormat, DXGI_FORMAT uavFormat,
	uint bindFlags, uint sampleCount, uint mipLevels)
{
	_size.w = width;
	_size.h = height;

	D3D11_TEXTURE2D_DESC textureDesc;
	memset(&textureDesc, 0, sizeof(D3D11_TEXTURE2D_DESC));

	// Setup the render target texture description.
	textureDesc.Width		= width;
	textureDesc.Height		= height;
	textureDesc.MipLevels	= mipLevels;
	textureDesc.ArraySize	= 1;
	auto GetDepthBufferTexDesc = [](DXGI_FORMAT foramt)
	{
		switch( foramt )
		{
		case DXGI_FORMAT_D32_FLOAT:
			return DXGI_FORMAT_R32_TYPELESS;
		case DXGI_FORMAT_D24_UNORM_S8_UINT:
			return DXGI_FORMAT_R24G8_TYPELESS;
		case DXGI_FORMAT_D16_UNORM:
			return DXGI_FORMAT_R16_TYPELESS;
		}

		return DXGI_FORMAT_UNKNOWN;
	};

	DXGI_FORMAT texFormat = (bindFlags & D3D11_BIND_DEPTH_STENCIL) ?  
							GetDepthBufferTexDesc(srvFormat) : srvFormat;
	assert(texFormat != DXGI_FORMAT_UNKNOWN); // "Error, cant support this format"

	textureDesc.Format			= texFormat;
	textureDesc.Usage			= D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags		= bindFlags;
	textureDesc.CPUAccessFlags	= 0;
	textureDesc.MiscFlags		= mipLevels > 1 ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;

	//multisampler
	{
		if(sampleCount == 0)
		{
			DXGI_SWAP_CHAIN_DESC swapChainDesc;
			dx.GetSwapChain()->GetDesc(&swapChainDesc);

			int count = (bindFlags & D3D11_BIND_UNORDERED_ACCESS) ? 1 : swapChainDesc.SampleDesc.Count;
			textureDesc.SampleDesc.Count = count;
		}
		else
		{
			textureDesc.SampleDesc.Count = sampleCount;
		}

		textureDesc.SampleDesc.Quality	= 0;//swapChainDesc.SampleDesc.Quality;
	}

	//Initialize texture
	_texture = dx.CreateTexture2D(textureDesc);

	if (bindFlags & D3D11_BIND_SHADER_RESOURCE)
	{
		ShaderResourceView srv;
		srv.InitializeUsingTexture(dx, _texture, srvFormat, mipLevels, (textureDesc.SampleDesc.Count > 1) ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D);

		_base.SetShaderResourceView(srv);
	}

	if (bindFlags & D3D11_BIND_UNORDERED_ACCESS)
	{
		UnorderedAccessView uav;
		uav.Initialize(dx, uavFormat, width * height, _texture, D3D11_UAV_DIMENSION_TEXTURE2D);

		_base.SetUnorderedAccessView(uav);
	}
}

const Size<uint>& Texture2D::FetchSize()
{
	if( (_size.w != 0) && (_size.h != 0) )
		return _size;

	D3D11_TEXTURE2D_DESC desc;
	if(_texture.IsCanUse())
	{
		_texture.GetRaw()->GetDesc(&desc);

		_size.w = desc.Width;
		_size.h = desc.Height;

		return _size;
	}

	ID3D11Resource* res = nullptr;
	_base.GetShaderResourceView().GetView().GetRaw()->GetResource(&res);

    ID3D11Texture2D* texture2d = nullptr;
    HRESULT hr = res->QueryInterface(&texture2d);

    if( SUCCEEDED(hr) )
    {
		texture2d->GetDesc(&desc);

		_size.w = desc.Width;
		_size.h = desc.Height;
	}

	SAFE_RELEASE(texture2d);
    SAFE_RELEASE(res);

	return _size;
}

void Texture2D::Destroy()
{
	_base.Destroy();
	_texture.Destroy();
	_size		= Size<uint>(0, 0);
}

void Texture2D::GenerateMips(Device::DirectX& dx)
{	
	dx.GetContext()->GenerateMips(_base.GetShaderResourceView().GetView().GetRaw());
}