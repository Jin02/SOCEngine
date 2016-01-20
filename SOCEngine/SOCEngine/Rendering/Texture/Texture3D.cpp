#include "Texture3D.h"
#include "Director.h"

using namespace Rendering::Texture;
using namespace Rendering::View;

Texture3D::Texture3D()
	: TextureForm(Type::Tex3D), _texture(nullptr), _size(0, 0, 0), _rtv(nullptr)
{
}

Texture3D::~Texture3D()
{
	Destory();
}

void Texture3D::Initialize(	uint width, uint height, uint depth,
							DXGI_FORMAT typelessFormat, DXGI_FORMAT srvFormat, DXGI_FORMAT uavFormat,
							uint optionBindFlag, uint mipLevels )
{
	_size.x = (float)width;
	_size.y = (float)height;
	_size.z = (float)depth;

	uint bindFlag = ((srvFormat != DXGI_FORMAT_UNKNOWN) ? D3D11_BIND_SHADER_RESOURCE	: 0) |
					((mipLevels > 1) ? D3D11_BIND_RENDER_TARGET	: 0) |	// D3D11_RESOURCE_MISC_GENERATE_MIPS을 사용하려면
																		// D3D11_BIND_RENDER_TARGET 설정된 상태여야 한다.
					((uavFormat != DXGI_FORMAT_UNKNOWN) ? D3D11_BIND_UNORDERED_ACCESS	: 0) | optionBindFlag;

	const Device::DirectX* dx = Device::Director::SharedInstance()->GetDirectX();
	ID3D11Device* device = dx->GetDevice();

	D3D11_TEXTURE3D_DESC textureDesc;
	memset(&textureDesc, 0, sizeof(D3D11_TEXTURE3D_DESC));

	// Setup the render target texture description.
	textureDesc.Width			= width;
	textureDesc.Height			= height;
	textureDesc.Depth			= depth;

	textureDesc.MipLevels		= mipLevels;
	textureDesc.MiscFlags		= mipLevels > 1 ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;

	textureDesc.Format			= typelessFormat;
	textureDesc.Usage			= D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags		= bindFlag;
	textureDesc.CPUAccessFlags	= 0;

	//Initialize texture
	{
		HRESULT hr = device->CreateTexture3D(&textureDesc, NULL, &_texture);
		ASSERT_COND_MSG(SUCCEEDED(hr), "Error, not create texture");
	}

	if(bindFlag & D3D11_BIND_SHADER_RESOURCE)
	{
		_srv = new ShaderResourceView;
		_srv->Initialize(_texture, srvFormat, mipLevels, D3D11_SRV_DIMENSION_TEXTURE3D);
	}

	if(bindFlag & D3D11_BIND_RENDER_TARGET)
	{
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
		rtvDesc.Format					= srvFormat;
		rtvDesc.ViewDimension			= D3D11_RTV_DIMENSION_TEXTURE3D;
		rtvDesc.Texture3D.MipSlice		= 0;
		rtvDesc.Texture3D.FirstWSlice	= 0;
		rtvDesc.Texture3D.WSize			= depth;
		HRESULT hr = device->CreateRenderTargetView(_texture, &rtvDesc, &_rtv);
		ASSERT_COND_MSG(SUCCEEDED(hr), "Error, can't create render target view");
	}

	if(bindFlag & D3D11_BIND_UNORDERED_ACCESS)
	{
		_uav = new UnorderedAccessView;
		_uav->Initialize(uavFormat, width * height * depth, _texture, D3D11_UAV_DIMENSION_TEXTURE3D, 0, depth);
	}
}

void Texture3D::Destory()
{
	TextureForm::Destroy();

	SAFE_RELEASE(_texture);
	SAFE_RELEASE(_rtv);
}