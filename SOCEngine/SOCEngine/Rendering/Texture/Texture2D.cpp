#include "Texture2D.h"
#include "Director.h"

using namespace Rendering::Texture;

Texture2D::Texture2D(ID3D11ShaderResourceView* srv, ID3D11Texture2D* tex, bool hasAlpha) 
	: _srv(srv), _hasAlpha(false), _texture(tex)
{
}

Texture2D::~Texture2D()
{
	SAFE_RELEASE(_srv);
	SAFE_RELEASE(_texture);
}

void Texture2D::Initialize(const Math::Size<unsigned int>& size, DXGI_FORMAT format, unsigned int bindFlags, unsigned int sampleCount)
{
	const Device::DirectX* dx = Device::Director::GetInstance()->GetDirectX();
	ID3D11Device* device = dx->GetDevice();

	D3D11_TEXTURE2D_DESC textureDesc;
	memset(&textureDesc, 0, sizeof(D3D11_TEXTURE2D_DESC));

	// Setup the render target texture description.
	textureDesc.Width = size.w;
	textureDesc.Height = size.h;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
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
							GetDepthBufferTexDesc(format) : format;
	ASSERT_COND_MSG(texFormat != DXGI_FORMAT_UNKNOWN, "Error, cant support this format");

	textureDesc.Format = texFormat;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = bindFlags;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	//multisampler
	{
		if(sampleCount == -1)
		{
			DXGI_SWAP_CHAIN_DESC swapChainDesc;
			dx->GetSwapChain()->GetDesc(&swapChainDesc);

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
	{
		HRESULT hr = device->CreateTexture2D(&textureDesc, NULL, &_texture);
		ASSERT_COND_MSG(SUCCEEDED(hr), "Error, not create texture");
	}

	if(bindFlags & D3D11_BIND_SHADER_RESOURCE)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srdesc;

		if( format == DXGI_FORMAT_D32_FLOAT )
			srdesc.Format = DXGI_FORMAT_R32_FLOAT;
		else if( format == DXGI_FORMAT_D16_UNORM)
			srdesc.Format = DXGI_FORMAT_R16_FLOAT;
		else if( format == DXGI_FORMAT_D24_UNORM_S8_UINT )
			srdesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
		else
			srdesc.Format = format;

		srdesc.ViewDimension = (textureDesc.SampleDesc.Count > 1) ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;
		srdesc.Texture2D.MostDetailedMip = 0;
		srdesc.Texture2D.MipLevels = 1;

		HRESULT hr = device->CreateShaderResourceView(_texture, &srdesc, &_srv);
		ASSERT_COND_MSG(SUCCEEDED(hr), "Error, not create shader resource view. plz check desc");
	}
}

Math::Size<uint> Texture2D::FetchSize() const
{
	D3D11_TEXTURE2D_DESC desc;
	if(_texture)
	{
		_texture->GetDesc(&desc);
		return Math::Size<uint>(desc.Width, desc.Height);
	}

	ID3D11Resource* res = nullptr;
	_srv->GetResource(&res);

    ID3D11Texture2D* texture2d = nullptr;
    HRESULT hr = res->QueryInterface(&texture2d);

	Math::Size<uint> size;
    if( SUCCEEDED(hr) )
    {
		texture2d->GetDesc(&desc);
		size.w = desc.Width;
		size.h = desc.Height;
	}

	SAFE_RELEASE(texture2d);
    SAFE_RELEASE(res);

	return size;
}