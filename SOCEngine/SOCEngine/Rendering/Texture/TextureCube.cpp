#include "TextureCube.h"
#include "Director.h"

using namespace Device;
using namespace Rendering;
using namespace Rendering::Texture;
using namespace Rendering::View;

TextureCube::TextureCube() : Texture2D(nullptr, nullptr, false), _rtv(nullptr)
{
}

TextureCube::~TextureCube()
{
}

void TextureCube::Initialize(const Math::Size<uint>& size, DXGI_FORMAT format, bool useRTV, bool useMipmap)
{
	_useMipmap = useMipmap;
	const uint bindFlags = D3D11_BIND_SHADER_RESOURCE | (useRTV ? D3D11_BIND_RENDER_TARGET : 0);

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

	// render target
	if(useRTV)
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
	SAFE_RELEASE(_rtv);

	Texture2D::Destroy();
}

void TextureCube::Clear(const Device::DirectX* dx)
{
	ID3D11DeviceContext* context = dx->GetContext();

	if(_rtv)
	{
		float clearColor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
		context->ClearRenderTargetView(_rtv, clearColor);
	}
}