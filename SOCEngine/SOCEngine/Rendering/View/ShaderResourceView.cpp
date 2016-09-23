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
	Destroy();
}

void ShaderResourceView::InitializeUsingTexture(
	ID3D11Resource* resource,
	DXGI_FORMAT format, uint mipLevel,
	D3D11_SRV_DIMENSION viewDimension)
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

	HRESULT hr = device->CreateShaderResourceView(resource, &srdesc, &_srv);
	ASSERT_COND_MSG(SUCCEEDED(hr), "Error, not create shader resource view. plz check desc");
}

void ShaderResourceView::InitializeUsingBuffer(ID3D11Buffer* buffer, uint num, DXGI_FORMAT format)
{
	const Device::DirectX* dx = Device::Director::SharedInstance()->GetDirectX();
	ID3D11Device* device = dx->GetDevice();

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.ElementOffset = 0;
	srvDesc.Buffer.ElementWidth = num;

	HRESULT hr = device->CreateShaderResourceView(buffer, &srvDesc, &_srv);
	ASSERT_COND_MSG(SUCCEEDED(hr), "Error!, does not create shader resource view");
}

void ShaderResourceView::Destroy()
{
	SAFE_RELEASE(_srv);
}