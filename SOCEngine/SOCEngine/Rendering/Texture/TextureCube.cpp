#include "TextureCube.h"

using namespace Device;
using namespace Rendering;
using namespace Rendering::Texture;
using namespace Rendering::View;

void TextureCube::Initialize(Device::DirectX& dx, const Size<uint>& size, DXGI_FORMAT format, bool useRTV, bool useMipmap)
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
	
	_texture = dx.CreateTexture2D(texDesc);

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

		_rtv = dx.CreateRenderTargetView(_texture.GetRaw(), rtvDesc);
	}

	if(bindFlags & D3D11_BIND_SHADER_RESOURCE)
		_srv.InitializeUsingTexture(dx, _texture, format, mipLevel, D3D11_SRV_DIMENSION_TEXTURECUBE);
}

void TextureCube::Clear(Device::DirectX& dx)
{
	float clearColor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	dx.GetContext()->ClearRenderTargetView(_rtv.GetRaw(), clearColor);
}
