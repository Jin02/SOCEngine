#include "RenderTextureCube.h"
#include "DirectX.h"

#undef max

using namespace Device;
using namespace Rendering;
using namespace Rendering::Texture;
using namespace Rendering::View;

void RenderTextureCube::Initialize(DirectX& dx, uint resolution, DXGI_FORMAT format)
{
	const uint bindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;

	auto Log2 = [](float f) -> uint
	{
		return static_cast<uint>(log(f) / log(2.0f));
	};
	const uint mipLevel = std::max(Log2(float(resolution + 1)), 1u);

	D3D11_TEXTURE2D_DESC texDesc;
	memset(&texDesc, 0, sizeof(D3D11_TEXTURE2D_DESC));

	texDesc.Width				= resolution;
	texDesc.Height				= resolution;
	texDesc.MipLevels			= mipLevel;
	texDesc.ArraySize			= 6;
	texDesc.Format				= format;
	texDesc.Usage				= D3D11_USAGE_DEFAULT;
	texDesc.BindFlags			= bindFlags;
	texDesc.CPUAccessFlags		= 0;
	texDesc.SampleDesc.Count	= 1;
	texDesc.SampleDesc.Quality	= 0;
	texDesc.MiscFlags			= D3D11_RESOURCE_MISC_TEXTURECUBE | D3D11_RESOURCE_MISC_GENERATE_MIPS;
	
	_tex2D.Initialize(dx, texDesc, format, DXGI_FORMAT_UNKNOWN);

	// render target
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	memset(&rtvDesc, 0, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
	rtvDesc.Format							= format;
	rtvDesc.ViewDimension					= D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	rtvDesc.Texture2DArray.FirstArraySlice	= 0;
	rtvDesc.Texture2DArray.ArraySize		= 6;
	rtvDesc.Texture2DArray.MipSlice			= 0;

	_renderTargetView = dx.CreateRenderTargetView(_tex2D.GetRawTexture(), rtvDesc);
}

void RenderTextureCube::Destroy()
{
	_tex2D.Destroy();
	_renderTargetView.Destroy();
}

void RenderTextureCube::GenerateMips(DirectX& dx)
{
	_tex2D.GenerateMips(dx);
}