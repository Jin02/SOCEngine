#include "PixelShader.h"
#include "Director.h"

using namespace Rendering::Shader;

PixelShader::PixelShader(ID3DBlob* blob) : BaseShader(blob), _shader(nullptr)
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

void PixelShader::SetShaderToContext(ID3D11DeviceContext* context)
{
	context->PSSetShader(_shader, nullptr, 0);
}

void PixelShader::UpdateResources(
	ID3D11DeviceContext* context,
	const std::vector<InputConstBuffer>* constBuffers,
	const std::vector<InputTexture>* textures,
	const std::vector<InputShaderResourceBuffer>* srBuffers)
{
	if(constBuffers)
	{
		for(auto iter = constBuffers->begin(); iter != constBuffers->end(); ++iter)
		{
			ID3D11Buffer* buffer = (*iter).buffer->GetBuffer();
			if(buffer && iter->usePS)
				context->PSSetConstantBuffers( (*iter).semanticIndex, 1, &buffer );
		}
	}

	if(textures)
	{
		for(auto iter = textures->begin(); iter != textures->end(); ++iter)
		{
			auto srv = iter->texture->GetShaderResourceView();
			if(srv && iter->usePS)
				context->PSSetShaderResources( iter->semanticIndex, 1, srv );
		}
	}

	if(srBuffers)
	{
		for(auto iter = srBuffers->begin(); iter != srBuffers->end(); ++iter)
		{
			auto srv = iter->srBuffer->GetShaderResourceView();
			if(srv && iter->usePS)
				context->PSSetShaderResources( iter->semanticIndex, 1, srv );
		}
	}
}

void PixelShader::Clear(
	ID3D11DeviceContext* context,
	const std::vector<InputConstBuffer>* constBuffers, 
	const std::vector<InputTexture>* textures,
	const std::vector<InputShaderResourceBuffer>* srBuffers)
{
	if(textures)
	{
		ID3D11ShaderResourceView* nullSrv = nullptr;

		for(auto iter = textures->begin(); iter != textures->end(); ++iter)
		{
			if(iter->usePS)
				context->VSSetShaderResources( iter->semanticIndex, 1, &nullSrv );
		}
	}

	if(srBuffers)
	{
		ID3D11ShaderResourceView* nullSrv = nullptr;

		for(auto iter = srBuffers->begin(); iter != srBuffers->end(); ++iter)
		{
			if(iter->usePS)
				context->VSSetShaderResources( iter->semanticIndex, 1, &nullSrv );
		}
	}

	if(constBuffers)
	{
		ID3D11Buffer* nullBuffer = nullptr;

		for(auto iter = constBuffers->begin(); iter != constBuffers->end(); ++iter)
		{
			if(iter->usePS)
				context->VSSetConstantBuffers( iter->semanticIndex, 1, &nullBuffer );
		}
	}
}