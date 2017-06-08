#include "ShaderResourceView.h"
#include <assert.h>

using namespace Rendering::View;
using namespace Math;

void ShaderResourceView::InitializeUsingTexture(
	Device::DirectX& dx,
	ID3D11Resource* resource,
	DXGI_FORMAT format, uint mipLevel,
	D3D11_SRV_DIMENSION viewDimension)
{
	assert(_srv.GetRaw() == nullptr);

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

	_srv = dx.CreateShaderResourceView(resource, srdesc);
}

void ShaderResourceView::InitializeUsingBuffer(Device::DirectX& dx, DXSharedResource<ID3D11Buffer>& buffer, uint num, DXGI_FORMAT format, bool isRawBuffer)
{
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	memset(&srvDesc, 0, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvDesc.Format = format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.ElementOffset = 0;
	srvDesc.Buffer.ElementWidth = num;
	srvDesc.BufferEx.Flags = isRawBuffer ? D3D11_BUFFEREX_SRV_FLAG_RAW : 0;

	_srv = dx.CreateShaderResourceView(buffer.GetRaw(), srvDesc);
}

void Rendering::View::ShaderResourceView::InitializeUsingBuffer(Device::DirectX& dx, const Buffer::BaseBuffer & buffer, uint num, DXGI_FORMAT format, bool isRawBuffer)
{
	InitializeUsingBuffer(dx, buffer, num, format, isRawBuffer);
}
