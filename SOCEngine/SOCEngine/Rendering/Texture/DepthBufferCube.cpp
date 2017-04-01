#include "DepthBufferCube.h"

using namespace Device;
using namespace Rendering;
using namespace Rendering::Texture;

void DepthBufferCube::Initialize(Device::DirectX& dx, const Size<uint>& size, bool useShaderResource)
{
	uint bindFlag =	D3D11_BIND_DEPTH_STENCIL | 
					(useShaderResource ? D3D11_BIND_SHADER_RESOURCE : 0);

	D3D11_TEXTURE2D_DESC texDesc;
	memset(&texDesc, 0, sizeof(D3D11_TEXTURE2D_DESC));

	texDesc.Width				= size.w;
	texDesc.Height				= size.h;
	texDesc.MipLevels			= 1;
	texDesc.ArraySize			= 6;
	texDesc.Format				= DXGI_FORMAT_D32_FLOAT;
	texDesc.Usage				= D3D11_USAGE_DEFAULT;
	texDesc.BindFlags			= bindFlag;
	texDesc.MiscFlags			= D3D11_RESOURCE_MISC_TEXTURECUBE;
	texDesc.CPUAccessFlags		= 0;
	texDesc.SampleDesc.Count	= 1;
	texDesc.SampleDesc.Quality	= 0;

	_texture = dx.CreateTexture2D(texDesc);

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	memset(&dsvDesc, 0, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	dsvDesc.Format				= DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension		= D3D11_DSV_DIMENSION_TEXTURE2DARRAY;

	dsvDesc.Texture2DArray.FirstArraySlice	= 0;
	dsvDesc.Texture2DArray.ArraySize		= 6;
	dsvDesc.Texture2DArray.MipSlice			= 0;

	_depthStencilView = dx.CreateDepthStencilView(_texture.GetRaw(), dsvDesc);
}

void DepthBufferCube::Clear(Device::DirectX& dx, float depth, unsigned char stencil)
{
	dx.GetContext()->ClearDepthStencilView(_depthStencilView.GetRaw(), D3D11_CLEAR_DEPTH, depth, stencil);
}