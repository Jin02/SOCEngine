#include "Texture3D.h"
#include "Director.h"

using namespace Rendering::Texture;

Texture3D::Texture3D(ID3D11ShaderResourceView* srv, ID3D11Texture3D* tex) 
	: _srv(srv), _texture(tex)
{
}

Texture3D::~Texture3D()
{
	SAFE_RELEASE(_srv);
	SAFE_RELEASE(_texture);
}

void Texture3D::Initialize(const Math::Size<unsigned int>& size, uint depth, DXGI_FORMAT format, unsigned int bindFlags, unsigned int mipLevels)
{
	const Device::DirectX* dx = Device::Director::GetInstance()->GetDirectX();
	ID3D11Device* device = dx->GetDevice();

	D3D11_TEXTURE3D_DESC textureDesc;
	memset(&textureDesc, 0, sizeof(D3D11_TEXTURE3D_DESC));

	// Setup the render target texture description.
	textureDesc.Width			= size.w;
	textureDesc.Height			= size.h;
	textureDesc.Depth			= depth;

	textureDesc.MipLevels		= mipLevels;
	textureDesc.MiscFlags		= mipLevels > 1 ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;

	textureDesc.Format			= format;
	textureDesc.Usage			= D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags		= bindFlags;
	textureDesc.CPUAccessFlags	= 0;

	//Initialize texture
	{
		HRESULT hr = device->CreateTexture3D(&textureDesc, NULL, &_texture);
		ASSERT_COND_MSG(SUCCEEDED(hr), "Error, not create texture");
	}

	if(bindFlags & D3D11_BIND_SHADER_RESOURCE)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srdesc;

		srdesc.Format = format;
		srdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
		srdesc.Texture3D.MostDetailedMip = 0;
		srdesc.Texture3D.MipLevels = mipLevels;

		HRESULT hr = device->CreateShaderResourceView(_texture, &srdesc, &_srv);
		ASSERT_COND_MSG(SUCCEEDED(hr), "Error, not create shader resource view. plz check desc");
	}
}

std::tuple<Math::Size<uint>, uint> Texture3D::FetchSize() const
{
	D3D11_TEXTURE3D_DESC desc;
	if(_texture)
	{
		_texture->GetDesc(&desc);
		return std::make_tuple(Math::Size<uint>(desc.Width, desc.Height), desc.Depth);
	}

	ID3D11Resource* res = nullptr;
	_srv->GetResource(&res);

    ID3D11Texture3D* texture2d = nullptr;
    HRESULT hr = res->QueryInterface(&texture2d);

    if( SUCCEEDED(hr) )
		texture2d->GetDesc(&desc);

	SAFE_RELEASE(texture2d);
    SAFE_RELEASE(res);

	return std::make_tuple(Math::Size<uint>(desc.Width, desc.Height), desc.Depth);;
}