#include "ShaderForm.h"

using namespace Rendering::Shader;
using namespace Rendering;

ShaderForm::ShaderForm(ID3DBlob* blob) : _blob(blob), _type(Type::Invalid)
{
}

ShaderForm::~ShaderForm(void)
{
}

ShaderForm::Usage::Usage()
	: useVS(false), usePS(false), useGS(false), useHS(false)
{

}

ShaderForm::Usage::Usage(bool _useVS, bool _useGS, bool _useHS, bool _usePS)
	: useVS(_useVS), usePS(_usePS), useGS(_useGS), useHS(_useHS)
{
}

void ShaderForm::Usage::SetUsage(ShaderForm::Usage usage)
{
	this->useVS = usage.useVS;
	this->usePS = usage.usePS;
	this->useGS = usage.useGS;
	this->useHS = usage.useHS;
}


ShaderForm::InputConstBuffer::InputConstBuffer()
	: Usage(), bindIndex(-1), buffer(nullptr)
{
}

ShaderForm::InputConstBuffer::InputConstBuffer(uint bindIndex, const Rendering::Buffer::ConstBuffer* buffer, bool useVS, bool useGS, bool useHS, bool usePS)
	: Usage(useVS, useGS, useHS, usePS)
{
	this->bindIndex = bindIndex;
	this->buffer = buffer;
}

ShaderForm::InputConstBuffer::InputConstBuffer(uint bindIndex, const Rendering::Buffer::ConstBuffer* buffer, Usage usage)
	: Usage(usage)
{
	this->bindIndex = bindIndex;
	this->buffer = buffer;
}


ShaderForm::InputTexture::InputTexture()
	: Usage(), bindIndex(-1), texture(nullptr)
{
}

ShaderForm::InputTexture::InputTexture(uint bindIndex, const Texture::Texture2D* texture, bool useVS, bool useGS, bool useHS, bool usePS)
	: Usage(useVS, useGS, useHS, usePS)
{
	this->bindIndex = bindIndex;
	this->texture = texture;
}

ShaderForm::InputTexture::InputTexture(uint bindIndex, const Texture::Texture2D* texture, Usage usage)
	: Usage(usage)
{
	this->bindIndex = bindIndex;
	this->texture = texture;
}


ShaderForm::InputShaderResourceBuffer::InputShaderResourceBuffer()
	: Usage(), bindIndex(-1), srBuffer(nullptr)
{
}

ShaderForm::InputShaderResourceBuffer::InputShaderResourceBuffer(uint bindIndex, const Buffer::ShaderResourceBuffer* srBuffer, bool useVS, bool useGS, bool useHS, bool usePS)
	: Usage(useVS, useGS, useHS, usePS)
{
	this->bindIndex = bindIndex;
	this->srBuffer = srBuffer;
}

ShaderForm::InputShaderResourceBuffer::InputShaderResourceBuffer(uint bindIndex, const Buffer::ShaderResourceBuffer* srBuffer, Usage usage)
	: Usage(usage)
{
	this->bindIndex = bindIndex;
	this->srBuffer = srBuffer;
}