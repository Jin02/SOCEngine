#include "PixelShader.h"

using namespace Rendering::Shader;

PixelShader::PixelShader(ID3DBlob* blob, const std::string& key)
	: ShaderForm(blob, key), _shader(nullptr)
{
	_type = Type::Pixel;
}

PixelShader::~PixelShader(void)
{
	SAFE_RELEASE(_shader);
}

bool PixelShader::Create(ID3D11Device* device)
{
	if(_blob == nullptr)
		return false;

	HRESULT hr = device->CreatePixelShader( _blob->GetBufferPointer(), _blob->GetBufferSize(), nullptr, &_shader);
	_blob->Release();

	if( FAILED( hr ) )
		return false;

	return true;
}

void PixelShader::BindShaderToContext(ID3D11DeviceContext* context)
{
	context->PSSetShader(_shader, nullptr, 0);
}

void PixelShader::BindResourcesToContext(
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
				context->PSSetConstantBuffers( (*iter).bindIndex, 1, &buffer );
		}
	}

	if(textures)
	{
		for(auto iter = textures->begin(); iter != textures->end(); ++iter)
		{
			auto srv = iter->texture->GetShaderResourceView()->GetView();
			if(srv && iter->usePS)
				context->PSSetShaderResources( iter->bindIndex, 1, &srv );
		}
	}

	if(srBuffers)
	{
		for(auto iter = srBuffers->begin(); iter != srBuffers->end(); ++iter)
		{
			auto srv = iter->srBuffer->GetShaderResourceView();
			if(srv && iter->usePS)
				context->PSSetShaderResources( iter->bindIndex, 1, srv );
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
				context->PSSetShaderResources( iter->bindIndex, 1, &nullSrv );
		}
	}

	if(srBuffers)
	{
		ID3D11ShaderResourceView* nullSrv = nullptr;

		for(auto iter = srBuffers->begin(); iter != srBuffers->end(); ++iter)
		{
			if(iter->usePS)
				context->PSSetShaderResources( iter->bindIndex, 1, &nullSrv );
		}
	}

	if(constBuffers)
	{
		ID3D11Buffer* nullBuffer = nullptr;

		for(auto iter = constBuffers->begin(); iter != constBuffers->end(); ++iter)
		{
			if(iter->usePS)
				context->PSSetConstantBuffers( iter->bindIndex, 1, &nullBuffer );
		}
	}
}

void PixelShader::BindTexture(ID3D11DeviceContext* context, TextureBindIndex bind, const Texture::Texture2D* tex)
{
	ID3D11ShaderResourceView* srv = tex ? tex->GetShaderResourceView()->GetView() : nullptr;
	context->PSSetShaderResources(uint(bind), 1, &srv);
}

void PixelShader::BindSamplerState(ID3D11DeviceContext* context, SamplerStateBindIndex bind, ID3D11SamplerState* samplerState)
{
	context->PSSetSamplers(uint(bind), 1, &samplerState);
}

void PixelShader::BindConstBuffer(ID3D11DeviceContext* context, ConstBufferBindIndex bind, const Buffer::ConstBuffer* cb)
{
	ID3D11Buffer* buf = cb ? cb->GetBuffer() : nullptr;
	context->PSSetConstantBuffers(uint(bind), 1, &buf);
}

void PixelShader::BindShaderResourceBuffer(ID3D11DeviceContext* context, TextureBindIndex bind, const Buffer::ShaderResourceBuffer* srBuffer)
{
	ID3D11ShaderResourceView* srv = srBuffer ? srBuffer->GetShaderResourceView() : nullptr;
	context->PSSetShaderResources(uint(bind), 1, &srv);
}
