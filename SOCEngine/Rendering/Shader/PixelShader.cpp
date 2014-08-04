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

void PixelShader::UpdateShader(const std::vector<BufferType>& constBuffers, const std::vector<TextureType>& textures, const SamplerType& sampler)
{
	ID3D11DeviceContext* context;
	context->PSSetShader(_shader, nullptr, 0);

	for(auto iter = constBuffers.begin(); iter != constBuffers.end(); ++iter)
	{
		ID3D11Buffer* buffer = (*iter).second->GetBuffer();
		context->PSSetConstantBuffers( (*iter).first, 1, &buffer );
	}

	for(auto iter = textures.begin(); iter != textures.end(); ++iter)
	{
		ID3D11ShaderResourceView* srv = (*iter).second->GetShaderResourceView();
		context->PSSetShaderResources( (*iter).first, 1, &srv );
	}

	ID3D11SamplerState* samplerState = sampler.second->GetSampler();
	context->PSSetSamplers(sampler.first, 1, &samplerState);
}