#include "VertexShader.h"
#include "Director.h"

using namespace Rendering::Shader;

VertexShader::VertexShader(ID3DBlob* blob) : Shader(blob), _shader(nullptr), _layout(nullptr)
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

	if( FAILED( hr ) )
		return false;

	_blob->Release();

	return true;
}

void VertexShader::UpdateShader(const std::vector<BufferType>& constBuffers, const std::vector<TextureType>& textures)
{
	static ID3D11DeviceContext* context = nullptr;
	context->IASetInputLayout(_layout);
	context->VSSetShader(_shader, nullptr, 0);

	for(auto iter = constBuffers.begin(); iter != constBuffers.end(); ++iter)
	{
		ID3D11Buffer* buffer = (*iter).second->GetBuffer();
		context->VSSetConstantBuffers( (*iter).first, 1, &buffer );
	}

	for(auto iter = textures.begin(); iter != textures.end(); ++iter)
	{
		ID3D11ShaderResourceView* srv = (*iter).second->GetShaderResourceView();
		context->VSSetShaderResources( (*iter).first, 1, &srv );
	}
}