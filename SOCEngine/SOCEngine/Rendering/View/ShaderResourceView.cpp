#include "ShaderResourceView.h"
#include "Director.h"

using namespace Rendering::View;
using namespace Math;

ShaderResourceView::ShaderResourceView(ID3D11ShaderResourceView* srv)
	: _srv(srv)
{

}

ShaderResourceView::~ShaderResourceView()
{
	Destory();
}

void ShaderResourceView::Initialize(
	ID3D11Resource* resource,
	DXGI_FORMAT format, uint mipLevel,
	D3D11_SRV_DIMENSION viewDimension,
	uint numElements)
{
	ASSERT_COND_MSG(_srv == nullptr, "Error, SRV was already allocated");

	const Device::DirectX* dx = Device::Director::SharedInstance()->GetDirectX();
	ID3D11Device* device = dx->GetDevice();

	D3D11_SHADER_RESOURCE_VIEW_DESC srdesc;
	memset(&srdesc, 0, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

	if( format == DXGI_FORMAT_D32_FLOAT )
		srdesc.Format = DXGI_FORMAT_R32_FLOAT;
	else if( format == DXGI_FORMAT_D16_UNORM)
		srdesc.Format = DXGI_FORMAT_R16_FLOAT;
	else if( format == DXGI_FORMAT_D24_UNORM_S8_UINT )
		srdesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	else
		srdesc.Format = format;

	srdesc.ViewDimension = viewDimension;

	if(viewDimension == D3D11_SRV_DIMENSION_TEXTURE2D)
		srdesc.Texture2D.MipLevels = mipLevel;
	else if(viewDimension == D3D11_SRV_DIMENSION_TEXTURE3D)
		srdesc.Texture3D.MipLevels = mipLevel;
	else if(viewDimension == D3D11_SRV_DIMENSION_TEXTURE1D)
		srdesc.Texture1D.MipLevels = mipLevel;

	if( numElements != -1 )
		srdesc.Buffer.NumElements = numElements;

	HRESULT hr = device->CreateShaderResourceView(resource, &srdesc, &_srv);
	ASSERT_COND_MSG(SUCCEEDED(hr), "Error, not create shader resource view. plz check desc");
}

void ShaderResourceView::Destory()
{
	SAFE_RELEASE(_srv);
}