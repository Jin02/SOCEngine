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

	void PixelShader::UpdateShader(ID3D11DeviceContext* context, const std::vector<BufferType>* rendererConstBuffers, const std::vector<BufferType>* materialConstBuffers, const std::vector<TextureType>* textures, const SamplerType& sampler)
	{
		context->PSSetShader(_shader, nullptr, 0);

		if(rendererConstBuffers)
		{
			for(auto iter = rendererConstBuffers->begin(); iter != rendererConstBuffers->end(); ++iter)
			{
				ID3D11Buffer* buffer = (*iter).second->GetBuffer();
				context->PSSetConstantBuffers( (*iter).first, 1, &buffer );
			}
		}

		if(materialConstBuffers)
		{
			for(auto iter = materialConstBuffers->begin(); iter != materialConstBuffers->end(); ++iter)
			{
				ID3D11Buffer* buffer = (*iter).second->GetBuffer();
				context->PSSetConstantBuffers( (*iter).first, 1, &buffer );
			}
		}

		if(textures)
		{
			for(auto iter = textures->begin(); iter != textures->end(); ++iter)
			{
				ID3D11ShaderResourceView* srv = iter->second->GetShaderResourceView();
				context->PSSetShaderResources( iter->first, 1, &srv );
			}
		}

		if(sampler.second)
		{
			ID3D11SamplerState* samplerState = sampler.second->GetSampler();
			context->PSSetSamplers(sampler.first, 1, &samplerState);
		}
	}
}