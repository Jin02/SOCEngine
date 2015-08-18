#include "CSRWBuffer.h"
#include "Director.h"

using namespace Device;
using namespace GPGPU::DirectCompute;

CSRWBuffer::CSRWBuffer() : CSOutput(), 
	_buffer(nullptr), _srv(nullptr)
{
}

CSRWBuffer::~CSRWBuffer()
{
	Destroy();
}

void CSRWBuffer::Initialize(DXGI_FORMAT format, uint stride, uint num)
{
	ID3D11Device* device = Director::GetInstance()->GetDirectX()->GetDevice();

	// buffer
	{
		D3D11_BUFFER_DESC desc;
		memset(&desc, 0, sizeof(D3D11_BUFFER_DESC));

		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = stride * num;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;

		HRESULT hr = device->CreateBuffer(&desc, nullptr, &_buffer);
		ASSERT_COND_MSG(SUCCEEDED( hr ), "Error!. does not create buffer");
	}

	// srv
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		memset(&desc, 0, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

		desc.Format = format;
		desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		desc.Buffer.ElementOffset = 0;
		desc.Buffer.ElementWidth = num;

		HRESULT hr = device->CreateShaderResourceView(_buffer, &desc, &_srv);
		ASSERT_COND_MSG(SUCCEEDED( hr ), "Error!. does not create srv");
	}

	CSOutput::Initialize(format, num, _buffer);	
}

void CSRWBuffer::Destroy()
{
	CSOutput::Destroy();

	SAFE_RELEASE(_srv);
	SAFE_RELEASE(_buffer);
}