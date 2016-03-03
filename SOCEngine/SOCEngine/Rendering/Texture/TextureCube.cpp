#include "TextureCube.h"
#include "Director.h"

using namespace Device;
using namespace Rendering;
using namespace Rendering::Texture;
using namespace Rendering::View;

TextureCube::TextureCube() : Texture2D(nullptr, nullptr, false),
	_depthMap(nullptr), _dsv(nullptr), _rtv(nullptr)
{
}

TextureCube::~TextureCube()
{
}

void TextureCube::Initialize(const Math::Size<uint>& size, DXGI_FORMAT format, bool useMipmap)
{
	_useMipmap = useMipmap;
	const uint bindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;

	uint bigSize = max(size.w, size.h);
	auto Log2 = [](float f) { return log(f) / log(2.0f); };
	const uint mipLevel		= max((uint)Log2(float(bigSize)) + 1, 1);

	D3D11_TEXTURE2D_DESC texDesc;
	memset(&texDesc, 0, sizeof(D3D11_TEXTURE2D_DESC));

	texDesc.Width				= size.w;
	texDesc.Height				= size.h;
	texDesc.MipLevels			= mipLevel;
	texDesc.ArraySize			= 6;
	texDesc.Format				= format;
	texDesc.Usage				= D3D11_USAGE_DEFAULT;
	texDesc.BindFlags			= bindFlags;
	texDesc.CPUAccessFlags		= 0;
	texDesc.SampleDesc.Count	= 1;
	texDesc.SampleDesc.Quality	= 0;

	uint miscMipFlag = (mipLevel > 1) ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;
	texDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE | miscMipFlag;

	const DirectX* dx = Device::Director::SharedInstance()->GetDirectX();
	ID3D11Device* device = dx->GetDevice();
	
	HRESULT hr = device->CreateTexture2D(&texDesc, nullptr, &_texture);
	ASSERT_COND_MSG(SUCCEEDED(hr), "Error, cant create texture");

	// Depth
	{
		texDesc.Format		= DXGI_FORMAT_D32_FLOAT;
	
		texDesc.BindFlags	= D3D11_BIND_DEPTH_STENCIL;
		texDesc.MiscFlags	= D3D11_RESOURCE_MISC_TEXTURECUBE;
		texDesc.MipLevels	= 1;
	
		hr = device->CreateTexture2D(&texDesc, nullptr, &_depthMap);
		ASSERT_COND_MSG(SUCCEEDED(hr), "Error, cant create texture");

		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
		memset(&dsvDesc, 0, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
		dsvDesc.Format			= DXGI_FORMAT_D32_FLOAT;
		dsvDesc.ViewDimension	= D3D11_DSV_DIMENSION_TEXTURE2DARRAY;

		dsvDesc.Texture2DArray.FirstArraySlice	= 0;
		dsvDesc.Texture2DArray.ArraySize		= 6;
		dsvDesc.Texture2DArray.MipSlice			= 0;

		hr = device->CreateDepthStencilView(_depthMap, &dsvDesc, &_dsv);
		ASSERT_COND_MSG(SUCCEEDED(hr), "Error, cant create dsv");
	}

	// render target
	{
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
		memset(&rtvDesc, 0, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
		rtvDesc.Format							= format;
		rtvDesc.ViewDimension					= D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
		rtvDesc.Texture2DArray.FirstArraySlice	= 0;
		rtvDesc.Texture2DArray.ArraySize		= 6;
		rtvDesc.Texture2DArray.MipSlice			= 0;

		hr = device->CreateRenderTargetView(_texture, &rtvDesc, &_rtv);
		ASSERT_COND_MSG(SUCCEEDED(hr), "Error, cant create rtv");
	}

	if(bindFlags & D3D11_BIND_SHADER_RESOURCE)
	{
		_srv = new ShaderResourceView;
		_srv->Initialize(_texture, format, mipLevel, D3D11_SRV_DIMENSION_TEXTURECUBE);
	}
}

void TextureCube::Destroy()
{
	SAFE_RELEASE(_depthMap);
	SAFE_RELEASE(_dsv);
	SAFE_RELEASE(_rtv);

	Texture2D::Destroy();
}