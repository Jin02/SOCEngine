#include "RenderTextureCube.h"
#include "DirectX.h"

using namespace Device;
using namespace Rendering;
using namespace Rendering::Texture;
using namespace Rendering::View;

void RenderTextureCube::Initialize(DirectX& dx, const Size<uint>& size, DXGI_FORMAT format)
{
	_size = size;

	const uint bindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;

	auto Log2 = [](float f) { return log(f) / log(2.0f); };
	const uint mipLevel = max((uint)Log2(static_cast<float>(max(size.w, size.h))) + 1, 1);

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
	texDesc.MiscFlags			= D3D11_RESOURCE_MISC_TEXTURECUBE | D3D11_RESOURCE_MISC_GENERATE_MIPS;
	
	_texture = dx.CreateTexture2D(texDesc);

	// render target
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	memset(&rtvDesc, 0, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
	rtvDesc.Format							= format;
	rtvDesc.ViewDimension					= D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	rtvDesc.Texture2DArray.FirstArraySlice	= 0;
	rtvDesc.Texture2DArray.ArraySize		= 6;
	rtvDesc.Texture2DArray.MipSlice			= 0;

	_renderTargetView = dx.CreateRenderTargetView(_texture.GetRaw(), rtvDesc);
	_srv.InitializeUsingTexture(dx, _texture, format, mipLevel, D3D11_SRV_DIMENSION_TEXTURECUBE);
}

void RenderTextureCube::Destroy()
{
	_texture.Destroy();
	_srv.Destroy();
	_size = Size<uint>(0, 0);
}

void RenderTextureCube::GenerateMips(DirectX& dx)
{
	dx.GetContext()->GenerateMips(_srv.GetRaw());
}