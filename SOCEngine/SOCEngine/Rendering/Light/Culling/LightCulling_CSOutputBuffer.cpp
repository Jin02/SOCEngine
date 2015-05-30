#include "LightCulling_CSOutputBuffer.h"
#include "Director.h"

using namespace Rendering;
using namespace Rendering::Light;

LightCulling_CSOutputBuffer::LightCulling_CSOutputBuffer() :
	_srv(nullptr)
{

}

LightCulling_CSOutputBuffer::~LightCulling_CSOutputBuffer()
{
	SAFE_RELEASE(_srv);
}

void LightCulling_CSOutputBuffer::Initialize(const Math::Size<unsigned int>& threadSize, unsigned int maxLightNumInTile)
{
	unsigned threadTotalSize = threadSize.w * threadSize.h;
	auto device = Device::Director::GetInstance()->GetDirectX()->GetDevice();

	//buffer
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory( &desc, sizeof(desc) );
		desc.Usage = D3D11_USAGE_DEFAULT;

		//4는,r32를 사용해서
		desc.ByteWidth = 4 * maxLightNumInTile * threadTotalSize;

		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		if( FAILED(device->CreateBuffer( &desc, NULL, &_buffer)) )
			ASSERT_MSG("Error!, not create buffer");
	}

	//srv
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		ZeroMemory( &desc, sizeof( D3D11_SHADER_RESOURCE_VIEW_DESC ) );
		desc.Format = DXGI_FORMAT_R32_UINT;
		desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		desc.Buffer.ElementOffset = 0;
		desc.Buffer.ElementWidth = maxLightNumInTile * threadTotalSize;
		if( FAILED( device->CreateShaderResourceView( _buffer, &desc, &_srv)))
			ASSERT_MSG("Error!, not create srv");
	}

	//uav
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
		ZeroMemory( &desc, sizeof( D3D11_UNORDERED_ACCESS_VIEW_DESC ) );
		desc.Format = DXGI_FORMAT_R32_UINT;
		desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		desc.Buffer.FirstElement = 0;
		desc.Buffer.NumElements = maxLightNumInTile * threadTotalSize;
		if( FAILED( device->CreateUnorderedAccessView( _buffer, &desc, &_uav ) ))
			ASSERT_MSG("Error!, not create uav");
	}
}