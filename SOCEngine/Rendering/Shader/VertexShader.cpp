#include "VertexShader.h"
#include "Director.h"

using namespace Rendering::Shader;

VertexShader::VertexShader() : Shader(), _shader(nullptr), _layout(nullptr)
{
	_type = Type::Vertex;
}

VertexShader::~VertexShader(void)
{
}

bool VertexShader::Create(const D3D11_INPUT_ELEMENT_DESC* vertexDeclations, unsigned int count)
{
	ID3D11Device* device = Device::Director::GetInstance()->GetDirectX()->GetDevice();

	HRESULT hr = device->CreateVertexShader( _blob->GetBufferPointer(), _blob->GetBufferSize(), nullptr, &_shader );

	if( FAILED( hr ) )
		return false;

	hr = device->CreateInputLayout(vertexDeclations, count,
		_blob->GetBufferPointer(), _blob->GetBufferSize(), &_layout);

	if( FAILED( hr ) )
		return false;

	_blob->Release();

	//ID3D11DeviceContext *context 
	//	= Device::Director::GetInstance()->GetDirectX()->GetContext();

	//context->IASetInputLayout( _layout );


	return true;
}