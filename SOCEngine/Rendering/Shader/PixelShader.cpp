#include "PixelShader.h"
#include "Director.h"

using namespace Rendering::Shader;

PixelShader::PixelShader(ID3DBlob* blob) : Shader(blob), _shader(nullptr)
{
	_type = Type::Pixel;
}

PixelShader::~PixelShader(void)
{
	SAFE_RELEASE(_shader);
}

bool PixelShader::CreateShader()
{
	if(_blob == nullptr)
		return false;

	ID3D11Device* device = Device::Director::GetInstance()->GetDirectX()->GetDevice();

	HRESULT hr = device->CreatePixelShader( _blob->GetBufferPointer(), _blob->GetBufferSize(), nullptr, &_shader);
	_blob->Release();

	if( FAILED( hr ) )
		return false;

	return true;
}

void PixelShader::Begin()
{
	ID3D11DeviceContext* context;
	context->PSSetShader(_shader, nullptr, 0);
}

void PixelShader::End()
{
	ID3D11DeviceContext* context;
	context->PSSetShader(nullptr, nullptr, 0);
}