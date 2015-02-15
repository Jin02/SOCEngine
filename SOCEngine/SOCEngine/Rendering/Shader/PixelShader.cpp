#include "PixelShader.h"
#include "Director.h"

namespace Rendering
{
	using namespace Shader;

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

	void PixelShader::UpdateShader(ID3D11DeviceContext* context)
	{
		context->PSSetShader(_shader, nullptr, 0);
	}

	void PixelShader::UpdateResources(ID3D11DeviceContext* context, const std::vector<BufferType>* constBuffers, const std::vector<TextureType>* textures, const std::vector<SamplerType>& samplers)
	{
		if(constBuffers)
		{
			for(auto iter = constBuffers->begin(); iter != constBuffers->end(); ++iter)
			{
				const Buffer::BaseBuffer* constBuffer = iter->second;
				if(constBuffer)
				{
					ID3D11Buffer* buffer = constBuffer->GetBuffer();
					if(buffer)
						context->PSSetConstantBuffers( (*iter).first, 1, &buffer );
				}
			}
		}

		if(textures)
		{
			for(auto iter = textures->begin(); iter != textures->end(); ++iter)
			{
				const Texture::Texture* tex = iter->second;
				if(tex)
				{
					ID3D11ShaderResourceView* srv = iter->second->GetShaderResourceView();
					if(srv)
						context->PSSetShaderResources( iter->first, 1, &srv );
				}
			}
		}

		for(auto iter = samplers.begin(); iter != samplers.end(); ++iter)
		{
			const Sampler* sampler = iter->second;

			if(sampler)
			{
				ID3D11SamplerState* samplerState = sampler->GetSampler();
				if(samplerState)
					context->PSSetSamplers(iter->first, 1, &samplerState);
			}
		}
	}

	void PixelShader::ClearResource(ID3D11DeviceContext* context, const std::vector<TextureType>* textures)
	{
		if(textures)
		{
			ID3D11ShaderResourceView* nullSrv = nullptr;

			for(auto iter = textures->begin(); iter != textures->end(); ++iter)
				context->PSSetShaderResources( iter->first, 1, &nullSrv );
		}
	}
}