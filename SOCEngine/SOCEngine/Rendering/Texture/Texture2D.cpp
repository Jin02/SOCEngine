#include "Texture2D.h"
#include <assert.h>
#include "DirectX.h"

using namespace Rendering::Texture;
using namespace Rendering::View;

Texture2D::Texture2D(const ShaderResourceView& srv, const DXSharedResource<ID3D11Texture2D>& tex, bool hasAlpha, const Size<uint>& size)
	: _srv(srv), _texture(tex), _size(size)
{
}

Texture2D::Texture2D(const DXSharedResource<ID3D11Texture2D>& tex, const Size<uint>& size)
	: _texture(tex), _size(size)
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
		D3D_SRV_DIMENSION dimension =	(textureDesc.ArraySize == 6)		? D3D_SRV_DIMENSION_TEXTURECUBE : 
										(textureDesc.SampleDesc.Count > 1)	? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;

		_srv.InitializeUsingTexture(dx, _texture, srvFormat, mipLevels, dimension);
	}

	if (bindFlags & D3D11_BIND_UNORDERED_ACCESS)
		_uav.Initialize(dx, uavFormat, width * height, _texture, D3D11_UAV_DIMENSION_TEXTURE2D);
}

void Texture2D::Initialize(Device::DirectX& dx, const D3D11_TEXTURE2D_DESC& texDesc, DXGI_FORMAT srvFormat, DXGI_FORMAT uavFormat)
{
	_size.w = texDesc.Width;
	_size.h = texDesc.Height;

	_texture = dx.CreateTexture2D(texDesc);

	if (texDesc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
	{
		D3D_SRV_DIMENSION dimension =	(texDesc.ArraySize == 6)		? D3D_SRV_DIMENSION_TEXTURECUBE : 
										(texDesc.SampleDesc.Count > 1)	? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;
		_srv.InitializeUsingTexture(dx, _texture, srvFormat, texDesc.MipLevels, dimension);
	}

	if (texDesc.BindFlags & D3D11_BIND_UNORDERED_ACCESS)
		_uav.Initialize(dx, uavFormat, _size.w * _size.h, _texture, D3D11_UAV_DIMENSION_TEXTURE2D);
}

void Texture2D::Destroy()
{
	_texture.Destroy();
	_srv.Destroy();
	_uav.Destroy();
	_size = Size<uint>(0, 0);
}

const Size<uint>& Texture2D::FetchSize()
{
	if( (_size.w != 0) & (_size.h != 0) )
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
	_srv.GetView().GetRaw()->GetResource(&res);

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

void Texture2D::GenerateMips(Device::DirectX& dx)
{	
	dx.GetContext()->GenerateMips(_srv.GetRaw());
}
