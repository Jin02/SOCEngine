#include "BaseShader.h"

using namespace Rendering::Shader;
using namespace Rendering;

BaseShader::BaseShader(ID3DBlob* blob) : _blob(blob), _type(Type::Invalid)
{
}

BaseShader::~BaseShader(void)
{
}

BaseShader::Usage::Usage(bool _useVS, bool _useGS, bool _useHS, bool _usePS)
	: useVS(_useVS), usePS(_usePS), useGS(_useGS), useHS(_useHS)
{
}

void BaseShader::Usage::SetUsage(BaseShader::Usage usage)
{
	this->useVS = usage.useVS;
	this->usePS = usage.usePS;
	this->useGS = usage.useGS;
	this->useHS = usage.useHS;
}


BaseShader::BufferType::BufferType(uint semanticIndex, const Rendering::Buffer::BaseBuffer* buffer, bool useVS, bool useGS, bool useHS, bool usePS)
	: Usage(useVS, useGS, useHS, usePS)
{
	this->semanticIndex = semanticIndex;
	this->buffer = buffer;
}

BaseShader::BufferType::BufferType(uint semanticIndex, const Rendering::Buffer::BaseBuffer* buffer, Usage usage)
	: Usage(usage)
{
	this->semanticIndex = semanticIndex;
	this->buffer = buffer;
}



BaseShader::TextureType::TextureType(uint semanticIndex, const Texture::Texture2D* texture, bool useVS, bool useGS, bool useHS, bool usePS)
	: Usage(useVS, useGS, useHS, usePS)
{
	this->semanticIndex = semanticIndex;
	this->texture = texture;
}

BaseShader::TextureType::TextureType(uint semanticIndex, const Texture::Texture2D* texture, Usage usage)
	: Usage(usage)
{
	this->semanticIndex = semanticIndex;
	this->texture = texture;
}



BaseShader::ShaderResourceType::ShaderResourceType(uint semanticIndex, const Buffer::ShaderResourceBuffer* srBuffer, bool useVS, bool useGS, bool useHS, bool usePS)
	: Usage(useVS, useGS, useHS, usePS)
{
	this->semanticIndex = semanticIndex;
	this->srBuffer = srBuffer;
}

BaseShader::ShaderResourceType::ShaderResourceType(uint semanticIndex, const Buffer::ShaderResourceBuffer* srBuffer, Usage usage)
	: Usage(usage)
{
	this->semanticIndex = semanticIndex;
	this->srBuffer = srBuffer;
}