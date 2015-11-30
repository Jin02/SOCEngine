#include "GeometryShader.h"
#include "Director.h"

using namespace Rendering::Shader;

GeometryShader::GeometryShader(ID3DBlob* blob)
	: ShaderForm(blob), _shader(nullptr)
{
	_type = Type::Geometry;
}

GeometryShader::~GeometryShader()
{
	SAFE_RELEASE(_shader);
}

bool GeometryShader::Create(ID3D11Device* device)
{
	if(_blob == nullptr)
		return false;

	HRESULT hr = device->CreateGeometryShader(_blob->GetBufferPointer(), _blob->GetBufferSize(), nullptr, &_shader);

	if( FAILED(hr) )
		return false;

	return true;
}

void GeometryShader::BindShaderToContext(ID3D11DeviceContext* context)
{
	context->GSSetShader(_shader, nullptr, 0);
}

void GeometryShader::BindResourcesToContext(ID3D11DeviceContext* context,
								   const std::vector<InputConstBuffer>* constBuffers,
								   const std::vector<InputTexture>* textures,
								   const std::vector<InputShaderResourceBuffer>* srBuffers)
{
	if(constBuffers)
	{
		for(auto iter = constBuffers->begin(); iter != constBuffers->end(); ++iter)
		{
			ID3D11Buffer* buffer = (*iter).buffer->GetBuffer();
			if(buffer && iter->useGS)
				context->GSSetConstantBuffers( (*iter).bindIndex, 1, &buffer );
		}
	}

	if(textures)
	{
		for(auto iter = textures->begin(); iter != textures->end(); ++iter)
		{
			auto srv = iter->texture->GetShaderResourceView()->GetView();
			if(srv && iter->useGS)
				context->GSSetShaderResources( iter->bindIndex, 1, &srv );
		}
	}

	if(srBuffers)
	{
		for(auto iter = srBuffers->begin(); iter != srBuffers->end(); ++iter)
		{
			auto srv = iter->srBuffer->GetShaderResourceView();
			if(srv && iter->useGS)
				context->GSSetShaderResources( iter->bindIndex, 1, srv );
		}
	}
}

void GeometryShader::Clear(ID3D11DeviceContext* context,
						   const std::vector<InputConstBuffer>* constBuffers,
						   const std::vector<InputTexture>* textures,
						   const std::vector<InputShaderResourceBuffer>* srBuffers)
{
	if(textures)
	{
		ID3D11ShaderResourceView* nullSrv = nullptr;

		for(auto iter = textures->begin(); iter != textures->end(); ++iter)
		{
			if(iter->useGS)
				context->GSSetShaderResources( iter->bindIndex, 1, &nullSrv );
		}
	}

	if(srBuffers)
	{
		ID3D11ShaderResourceView* nullSrv = nullptr;

		for(auto iter = srBuffers->begin(); iter != srBuffers->end(); ++iter)
		{
			if(iter->useGS)
				context->GSSetShaderResources( iter->bindIndex, 1, &nullSrv );
		}
	}

	if(constBuffers)
	{
		ID3D11Buffer* nullBuffer = nullptr;

		for(auto iter = constBuffers->begin(); iter != constBuffers->end(); ++iter)
		{
			if(iter->useGS)
				context->GSSetConstantBuffers( iter->bindIndex, 1, &nullBuffer );
		}
	}
}