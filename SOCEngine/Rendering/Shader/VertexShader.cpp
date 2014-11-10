#include "VertexShader.h"
#include "Director.h"

namespace Rendering
{
	using namespace Shader;

	VertexShader::VertexShader(ID3DBlob* blob) : BaseShader(blob), _shader(nullptr), _layout(nullptr)
	{
		_type = Type::Vertex;
	}

	VertexShader::~VertexShader(void)
	{
		SAFE_RELEASE(_shader);
	}

	bool VertexShader::CreateShader(const D3D11_INPUT_ELEMENT_DESC* vertexDeclations, unsigned int count)
	{
		if(_blob == nullptr)
			return false;

		ID3D11Device* device = Device::Director::GetInstance()->GetDirectX()->GetDevice();

		HRESULT hr = device->CreateVertexShader( _blob->GetBufferPointer(), _blob->GetBufferSize(), nullptr, &_shader );

		if( FAILED( hr ) )
			return false;

		hr = device->CreateInputLayout(vertexDeclations, count,
			_blob->GetBufferPointer(), _blob->GetBufferSize(), &_layout);

		_blob->Release();

		for(unsigned int i=0; i<count; ++i)
		{
			const D3D11_INPUT_ELEMENT_DESC& desc = vertexDeclations[i];
			std::string semanticName = desc.SemanticName;

			if(semanticName == "TEXCOORD")
				semanticName += ('0' + desc.SemanticIndex);

			SemanticInfo info;
			info.name = semanticName;

			if( (i+1) != count )
				info.size = vertexDeclations[i+1].AlignedByteOffset - desc.AlignedByteOffset;

			_semanticInfo.push_back(info);
		}

		unsigned int& size = _semanticInfo.back().size;
		switch(vertexDeclations[count-1].Format)
		{
			case DXGI_FORMAT_R32G32B32_FLOAT:
			case DXGI_FORMAT_R32G32B32_UINT:
			case DXGI_FORMAT_R32G32B32_SINT:
				size = 12;
				break;

			case DXGI_FORMAT_R32G32_FLOAT:
			case DXGI_FORMAT_R32G32_UINT:
			case DXGI_FORMAT_R32G32_SINT:
				size = 8;
				break;

			case DXGI_FORMAT_R32_FLOAT:
			case DXGI_FORMAT_R32_UINT:
			case DXGI_FORMAT_R32_SINT:
				size = 4;
				break;
		}

		if( FAILED( hr ) )
			return false;

		return true;
	}

	void VertexShader::UpdateShader(ID3D11DeviceContext* context, const std::vector<BufferType>* constBuffers, const std::vector<const Texture::Texture*>* textures)
	{
		context->IASetInputLayout(_layout);
		context->VSSetShader(_shader, nullptr, 0);

		if(constBuffers)
		{
			for(auto iter = constBuffers->begin(); iter != constBuffers->end(); ++iter)
			{
				ID3D11Buffer* buffer = (*iter).second->GetBuffer();
				context->VSSetConstantBuffers( (*iter).first, 1, &buffer );
			}
		}

		if(textures)
		{
			unsigned int index = 0;
			for(auto iter = textures->begin(); iter != textures->end(); ++iter, ++index)
			{
				ID3D11ShaderResourceView* srv = (*iter)->GetShaderResourceView();
				context->VSSetShaderResources( index, 1, &srv );
			}
		}
	}
}