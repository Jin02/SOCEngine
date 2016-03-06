#include "DepthBufferCube.h"
#include "Director.h"

using namespace Device;
using namespace Rendering;
using namespace Rendering::Texture;

DepthBufferCube::DepthBufferCube() 
	: Texture2D(nullptr, nullptr, false), _depthStencilView(nullptr)
{
}

DepthBufferCube::~DepthBufferCube()
{
	Destroy();
}

bool DepthBufferCube::Initialize(const Math::Size<unsigned int>& size, bool useShaderResource)
{
	unsigned int bindFlag =	D3D11_BIND_DEPTH_STENCIL | 
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

	const DirectX* dx = Device::Director::SharedInstance()->GetDirectX();
	ID3D11Device* device = dx->GetDevice();

	HRESULT hr = device->CreateTexture2D(&texDesc, nullptr, &_texture);
	ASSERT_COND_MSG(SUCCEEDED(hr), "Error, cant create texture");

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	memset(&dsvDesc, 0, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	dsvDesc.Format				= DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension		= D3D11_DSV_DIMENSION_TEXTURE2DARRAY;

	dsvDesc.Texture2DArray.FirstArraySlice	= 0;
	dsvDesc.Texture2DArray.ArraySize		= 6;
	dsvDesc.Texture2DArray.MipSlice			= 0;

	hr = device->CreateDepthStencilView(_texture, &dsvDesc, &_depthStencilView);
	ASSERT_COND_MSG(SUCCEEDED(hr), "Error, cant create dsv");

	return true;
}

void DepthBufferCube::Destroy()
{
	SAFE_RELEASE(_depthStencilView);
}

void DepthBufferCube::Clear(ID3D11DeviceContext* context, float depth, unsigned char stencil)
{
	context->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH, depth, stencil);
}