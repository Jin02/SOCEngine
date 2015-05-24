#include "Texture.h"
#include "Director.h"

using namespace Rendering::Texture;

Texture::Texture(ID3D11ShaderResourceView* srv, bool hasAlpha) 
	: _srv(srv), _hasAlpha(false), _texture(nullptr)
{
}

Texture::~Texture()
{
	SAFE_RELEASE(_srv);
	SAFE_RELEASE(_texture);
}

void Texture::Create(const Math::Size<unsigned int>& size, DXGI_FORMAT format, unsigned int bindFlags)
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
	textureDesc.Format = bindFlags & D3D11_BIND_DEPTH_STENCIL ?  GetDepthBufferTexDesc(format) : format;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = bindFlags;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	//multisampler
	{
		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		dx->GetSwapChain()->GetDesc(&swapChainDesc);
		textureDesc.SampleDesc.Count	= swapChainDesc.SampleDesc.Count;
		textureDesc.SampleDesc.Quality	= swapChainDesc.SampleDesc.Quality;
	}

	//Create texture
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

		srdesc.ViewDimension = textureDesc.SampleDesc.Count > 1 ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;
		srdesc.Texture2D.MostDetailedMip = 0;
		srdesc.Texture2D.MipLevels = 1;

		HRESULT hr = device->CreateShaderResourceView(_texture, &srdesc, &_srv);
		ASSERT_COND_MSG(SUCCEEDED(hr), "Error, not create shader resource view. plz check desc");
	}
}
