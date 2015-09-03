#include "VertexShader.h"
#include "Director.h"

using namespace Device;
using namespace Rendering::Shader;


VertexShader::VertexShader(ID3DBlob* blob) : ShaderForm(blob), _shader(nullptr), _layout(nullptr)
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

	if(vertexDeclations.size() == 0)
		return true;

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

void VertexShader::SetShaderToContext(ID3D11DeviceContext* context)
{
	context->VSSetShader(_shader, nullptr, 0);
}

void VertexShader::SetInputLayoutToContext(ID3D11DeviceContext* context)
{
	context->IASetInputLayout(_layout);
}

void VertexShader::UpdateResources(
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
			if(buffer && iter->useVS)
				context->VSSetConstantBuffers( (*iter).semanticIndex, 1, &buffer );
		}
	}

	if(textures)
	{
		for(auto iter = textures->begin(); iter != textures->end(); ++iter)
		{
			auto srv = iter->texture->GetShaderResourceView();
			if(srv && iter->useVS)
				context->VSSetShaderResources( iter->semanticIndex, 1, srv );
		}
	}

	if(srBuffers)
	{
		for(auto iter = srBuffers->begin(); iter != srBuffers->end(); ++iter)
		{
			auto srv = iter->srBuffer->GetShaderResourceView();
			if(srv && iter->useVS)
				context->VSSetShaderResources( iter->semanticIndex, 1, srv );
		}
	}
}

void VertexShader::Clear(
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
			if(iter->useVS)
				context->VSSetShaderResources( iter->semanticIndex, 1, &nullSrv );
		}
	}

	if(srBuffers)
	{
		ID3D11ShaderResourceView* nullSrv = nullptr;

		for(auto iter = srBuffers->begin(); iter != srBuffers->end(); ++iter)
		{
			if(iter->useVS)
				context->VSSetShaderResources( iter->semanticIndex, 1, &nullSrv );
		}
	}

	if(constBuffers)
	{
		ID3D11Buffer* nullBuffer = nullptr;

		for(auto iter = constBuffers->begin(); iter != constBuffers->end(); ++iter)
		{
			if(iter->useVS)
				context->VSSetConstantBuffers( iter->semanticIndex, 1, &nullBuffer );
		}
	}
}
