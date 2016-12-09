#include "Material.h"
#include "Utility.h"
#include "Director.h"

using namespace Rendering;
using namespace Rendering::Buffer;
using namespace Rendering::Shader;
using namespace Rendering::Texture;

Material::CustomShader::CustomShader() : isDeferred(false), shaderGroup()
{
}

Material::CustomShader::CustomShader(bool isDeferred, const Shader::ShaderGroup& shaderGroup)
{
	this->isDeferred = isDeferred;
	this->shaderGroup = shaderGroup;
}

Material::CustomShader::~CustomShader()
{
}

Material::Material(const std::string& name, Type type)	
	: _name(name), _hasAlpha(false), _tiling(1.0f, 1.0f), _type(type),
	_variableUpdateCounter(0), _alpha(1.0f)
{
}

Material::~Material(void)
{

}

void Material::Initialize(){}
void Material::Destroy(){}
void Material::UpdateConstBuffer(const Device::DirectX* dx){}

const Texture2D* Material::FindTexture(unsigned int& outArrayIndex, unsigned int bindIndex)
{	
	for(unsigned int i=0; i<_textures.size(); ++i)
	{		
		if(_textures[i].bindIndex == bindIndex)
		{
			outArrayIndex = i;
			return dynamic_cast<const Texture2D*>(_textures[i].texture);
		}
	}

	outArrayIndex = -1;
	return nullptr;
}

void Material::SetTextureUseBindIndex(unsigned int bindIndex, const Rendering::Texture::Texture2D* texture, ShaderForm::Usage usage)
{
	unsigned int arrayIdx = 0;
	auto hasTexture = FindTexture(arrayIdx, bindIndex);

	if(hasTexture)
	{
		_textures[arrayIdx].texture = texture;
	}
	else
	{
		_textures.push_back(ShaderForm::InputTexture(bindIndex, texture, usage));
	}
}

void Material::SetTextureUseArrayIndex(unsigned int arrayIndex, const Rendering::Texture::Texture2D* texture, ShaderForm::Usage usage)
{
	ASSERT_MSG_IF(arrayIndex < _textures.size(), "Error, Invalid arrayIdx");

	_textures[arrayIndex].texture = texture;
	_textures[arrayIndex].SetUsage(usage);
}

void Material::SetConstBufferUseBindIndex(uint bindIndex, const Buffer::ConstBuffer* cb, ShaderForm::Usage usage)
{
	unsigned int arrayIdx = 0;
	auto has = FindConstBuffer(arrayIdx, bindIndex);

	if(has)
	{
		_constBuffers[arrayIdx].buffer = cb;
	}
	else
	{
		_constBuffers.push_back(ShaderForm::InputConstBuffer(bindIndex, cb, usage));
	}
}

void Material::SetConstBufferUseArrayIndex(uint arrayIdx, const Buffer::ConstBuffer* cb, ShaderForm::Usage usage)
{
	ASSERT_MSG_IF(arrayIdx < _constBuffers.size(), "Error, Invalid arrayIdx");

	_constBuffers[arrayIdx].buffer = cb;
	_constBuffers[arrayIdx].SetUsage(usage);
}

const Buffer::ConstBuffer* Material::FindConstBuffer(unsigned int& outArrayIndex, unsigned int bindIndex)
{
	for(unsigned int i=0; i<_constBuffers.size(); ++i)
	{		
		if(_constBuffers[i].bindIndex == bindIndex)
		{
			outArrayIndex = i;
			return dynamic_cast<const Buffer::ConstBuffer*>(_constBuffers[i].buffer);
		}
	}

	outArrayIndex = 0;
	return nullptr;
}

const ShaderResourceBuffer* Material::FindShaderResourceBuffer(unsigned int& outArrayIndex, unsigned int bindIndex)
{
	for(unsigned int i=0; i<_srBuffers.size(); ++i)
	{		
		if(_srBuffers[i].bindIndex == bindIndex)
		{
			outArrayIndex = i;
			return dynamic_cast<const ShaderResourceBuffer*>(_srBuffers[i].srBuffer);
		}
	}

	outArrayIndex = 0;
	return nullptr;
}

void Material::SetShaderResourceBufferUseBindIndex(unsigned int bindIndex, const Rendering::Buffer::ShaderResourceBuffer* srBuffer, ShaderForm::Usage usage)
{
	unsigned int arrayIdx = 0;
	auto has = FindConstBuffer(arrayIdx, bindIndex);

	if(has)
	{
		_srBuffers[arrayIdx].srBuffer = srBuffer;
	}
	else
	{
		_srBuffers.push_back(ShaderForm::InputShaderResourceBuffer(bindIndex, srBuffer, usage));
	}
}

void Material::SetShaderResourceBufferUseArrayIndex(unsigned int arrayIndex, const Rendering::Buffer::ShaderResourceBuffer* srBuffer, ShaderForm::Usage usage)
{
	ASSERT_MSG_IF(arrayIndex < _srBuffers.size(), "Error, Invalid arrayIdx");

	_srBuffers[arrayIndex].srBuffer = srBuffer;
	_srBuffers[arrayIndex].SetUsage(usage);
}