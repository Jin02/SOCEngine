#include "VertexShader.h"
#include "Director.h"

using namespace Device;

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

	bool VertexShader::CreateShader(const std::vector<D3D11_INPUT_ELEMENT_DESC>& vertexDeclations)
	{
		if(_blob == nullptr)
			return false;

		uint count = vertexDeclations.size();

		const DirectX* dx = Director::GetInstance()->GetDirectX();
		ID3D11Device* device = dx->GetDevice();

		HRESULT hr = device->CreateVertexShader( _blob->GetBufferPointer(), _blob->GetBufferSize(), nullptr, &_shader );

		if( FAILED( hr ) )
			return false;

		hr = device->CreateInputLayout(vertexDeclations.data(), count,
			_blob->GetBufferPointer(), _blob->GetBufferSize(), &_layout);

		_blob->Release();

		if( FAILED( hr ) )
			return false;

		for(unsigned int i=0; i<count; ++i)
		{
			const D3D11_INPUT_ELEMENT_DESC& desc = vertexDeclations[i];

			SemanticInfo info;
			{
				info.name = desc.SemanticName;
				info.semanticIndex = desc.SemanticIndex;

				if( (i+1) != count )
					info.size = vertexDeclations[i+1].AlignedByteOffset - desc.AlignedByteOffset;
			}

			_semanticInfo.push_back(info);
		}

		_semanticInfo.back().size = dx->CalcFormatSize(vertexDeclations[count-1].Format);

		return true;
	}

	void VertexShader::UpdateShader(ID3D11DeviceContext* context)
	{
		context->VSSetShader(_shader, nullptr, 0);
	}

	void VertexShader::UpdateInputLayout(ID3D11DeviceContext* context)
	{
		context->IASetInputLayout(_layout);
	}

	void VertexShader::UpdateResources(ID3D11DeviceContext* context, const std::vector<BufferType>* constBuffers, const std::vector<TextureType>* textures)
	{
		if(constBuffers)
		{
			for(auto iter = constBuffers->begin(); iter != constBuffers->end(); ++iter)
			{
				ID3D11Buffer* buffer = (*iter).second->GetBuffer();
				if(buffer)
					context->VSSetConstantBuffers( (*iter).first, 1, &buffer );
			}
		}

		if(textures)
		{
			for(auto iter = textures->begin(); iter != textures->end(); ++iter)
			{
				ID3D11ShaderResourceView* srv = iter->second->GetShaderResourceView();
				if(srv)
					context->VSSetShaderResources( iter->first, 1, &srv );
			}
		}
	}
	
	void VertexShader::ClearResource(ID3D11DeviceContext* context, const std::vector<TextureType>* textures)
	{
		if(textures)
		{
			ID3D11ShaderResourceView* nullSrv = nullptr;

			for(auto iter = textures->begin(); iter != textures->end(); ++iter)
				context->VSSetShaderResources( iter->first, 1, &nullSrv );
		}
	}
}