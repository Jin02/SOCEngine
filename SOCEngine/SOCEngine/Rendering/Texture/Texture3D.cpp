#include "Texture3D.h"

using namespace Rendering::Texture;
using namespace Rendering::View;
using namespace Math;

Texture3D::Texture3D()
	: _base(), _texture(nullptr), _size(0, 0, 0), _rtv(nullptr)
{
}

Texture3D::~Texture3D()
{
	Destroy();
}

void Texture3D::Initialize(	Device::DirectX& dx,
							uint width, uint height, uint depth,
							DXGI_FORMAT typelessFormat, DXGI_FORMAT srvFormat, DXGI_FORMAT uavFormat,
							uint optionBindFlag, uint mipLevels )
{
	_size = Vector3(static_cast<float>(width), static_cast<float>(height), static_cast<float>(depth));

	uint bindFlag = ((srvFormat != DXGI_FORMAT_UNKNOWN) ? D3D11_BIND_SHADER_RESOURCE	: 0) |
					((mipLevels > 1) ? D3D11_BIND_RENDER_TARGET	: 0) |	// D3D11_RESOURCE_MISC_GENERATE_MIPS을 사용하려면
																		// D3D11_BIND_RENDER_TARGET 설정된 상태여야 한다.
					((uavFormat != DXGI_FORMAT_UNKNOWN) ? D3D11_BIND_UNORDERED_ACCESS	: 0) | optionBindFlag;

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
	_texture = dx.CreateTexture3D(textureDesc);

	if(bindFlag & D3D11_BIND_RENDER_TARGET)
	{
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
		rtvDesc.Format					= srvFormat;
		rtvDesc.ViewDimension			= D3D11_RTV_DIMENSION_TEXTURE3D;
		rtvDesc.Texture3D.MipSlice		= 0;
		rtvDesc.Texture3D.FirstWSlice	= 0;
		rtvDesc.Texture3D.WSize			= -1;

		_rtv = dx.CreateRenderTargetView(_texture.GetRaw(), rtvDesc);
	}

	if (bindFlag & D3D11_BIND_SHADER_RESOURCE)
	{
		ShaderResourceView srv;
		srv.InitializeUsingTexture(dx, _texture, srvFormat, mipLevels, D3D11_SRV_DIMENSION_TEXTURE3D);

		_base.SetShaderResourceView(srv);
	}

	if (bindFlag & D3D11_BIND_UNORDERED_ACCESS)
	{
		UnorderedAccessView uav;
		uav.Initialize(dx, uavFormat, width * height * depth, _texture, D3D11_UAV_DIMENSION_TEXTURE3D, 0, -1);

		_base.SetUnorderedAccessView(uav);
	}
}

void Texture3D::Destroy()
{
	_base.Destroy();
	_texture.Destroy();
	_rtv.Destroy();
	_size = Vector3(0.0f, 0.0f, 0.0f);
}