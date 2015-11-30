#include "Texture3D.h"
#include "Director.h"

using namespace Rendering::Texture;
using namespace Rendering::View;

Texture3D::Texture3D()
	: TextureForm(Type::Tex3D), _texture(nullptr), _size(0, 0, 0)
{
}

Texture3D::~Texture3D()
{
	Destory();
}

void Texture3D::Initialize(uint width, uint height, uint depth, DXGI_FORMAT format, unsigned int bindFlags, unsigned int mipLevels)
{
	_size.x = (float)width;
	_size.y = (float)height;
	_size.z = (float)depth;

	const Device::DirectX* dx = Device::Director::GetInstance()->GetDirectX();
	ID3D11Device* device = dx->GetDevice();

	D3D11_TEXTURE3D_DESC textureDesc;
	memset(&textureDesc, 0, sizeof(D3D11_TEXTURE3D_DESC));

	// Setup the render target texture description.
	textureDesc.Width			= width;
	textureDesc.Height			= height;
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
		_srv = new ShaderResourceView;
		_srv->Initialize(_texture, format, mipLevels, D3D11_SRV_DIMENSION_TEXTURE3D);
	}

	if(bindFlags & D3D11_BIND_UNORDERED_ACCESS)
	{
		_uav = new UnorderedAccessView;
		_uav->Initialize(format, width * height * depth, _texture, D3D11_UAV_DIMENSION_TEXTURE3D);
	}
}

void Texture3D::Destory()
{
	TextureForm::Destroy();

	SAFE_RELEASE(_texture);
}