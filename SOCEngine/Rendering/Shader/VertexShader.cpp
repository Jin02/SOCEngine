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

void VertexShader::Begin()
{
	static ID3D11DeviceContext* context = nullptr;
	context->IASetInputLayout(_layout);
	context->VSSetShader(_shader, nullptr, 0);
}

void VertexShader::End()
{
	static ID3D11DeviceContext* context = nullptr;
	context->IASetInputLayout(nullptr);
	context->VSSetShader(nullptr, nullptr, 0);
}