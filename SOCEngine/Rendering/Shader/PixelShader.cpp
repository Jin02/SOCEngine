#include "PixelShader.h"
#include "Director.h"

using namespace Rendering::Shader;

PixelShader::PixelShader() : Shader(), _shader(nullptr), _layout(nullptr)
{
}

PixelShader::~PixelShader(void)
{
}

bool PixelShader::Create()
{
	ID3D11Device* device = Device::Director::GetInstance()->GetDirectX()->GetDevice();

	HRESULT hr = device->CreatePixelShader( _blob->GetBufferPointer(), _blob->GetBufferSize(), nullptr, &_shader);
	_blob->Release();

	if( FAILED( hr ) )
		return false;



	return true;
}