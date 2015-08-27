#include "Material.h"
#include "Utility.h"
#include "Director.h"

using namespace Rendering;
using namespace Rendering::Buffer;
using namespace Rendering::Shader;

Material::Material(const std::string& name, Type type)	
	: _name(name), _hasAlpha(false), _changedAlpha(true),
	_tiling(1.0f, 1.0f), _type(type), _variableUpdateCounter(0)
{
}

Material::~Material(void)
{

}

const Rendering::Texture::Texture2D* Material::FindTexture(unsigned int& outArrayIndex, unsigned int shaderSlotIndex)
{	
	for(unsigned int i=0; i<_textures.size(); ++i)
	{		
		if(_textures[i].semanticIndex == shaderSlotIndex)
		{
			outArrayIndex = i;
			return _textures[i].texture;
		}
	}

	DEBUG_LOG("Material FindTexture Warning : Undefined UsageTextureType");
	outArrayIndex = 0;
	return nullptr;
}

bool Material::SetTextureUseShaderSlotIndex(unsigned int shaderSlotIndex, const Rendering::Texture::Texture2D* texture, BaseShader::Usage usage)
{
	unsigned int arrayIdx = 0;
	auto hasTexture = FindTexture(arrayIdx, shaderSlotIndex);

	if(hasTexture)
	{
		_textures[arrayIdx].texture = texture;
	}
	else
	{
		_textures.push_back(BaseShader::TextureType(shaderSlotIndex, texture, usage));
	}

	return hasTexture != nullptr;
}

bool Material::SetTextureUseArrayIndex(unsigned int arrayIndex, const Rendering::Texture::Texture2D* texture, BaseShader::Usage usage)
{
	if(arrayIndex >= _textures.size())
		return false;

	_textures[arrayIndex].texture = texture;
	_textures[arrayIndex].SetUsage(usage);
	return true;
}

bool Material::SetConstBufferUseShaderSlotIndex(uint shaderSlotIdx, const Buffer::ConstBuffer* cb, BaseShader::Usage usage)
{
	unsigned int arrayIdx = 0;
	auto has = FindConstBuffer(arrayIdx, shaderSlotIdx);

	if(has)
	{
		_constBuffers[arrayIdx].buffer = cb;
	}
	else
	{
		_constBuffers.push_back(BaseShader::BufferType(shaderSlotIdx, cb, usage));
	}

	return has != nullptr;
}

bool Material::SetConstBufferUseArrayIndex(uint arrayIdx, const Buffer::ConstBuffer* cb, BaseShader::Usage usage)
{
	if(arrayIdx >= _constBuffers.size())
		return false;

	_constBuffers[arrayIdx].buffer = cb;
	_constBuffers[arrayIdx].SetUsage(usage);
	return true;
}

const Buffer::ConstBuffer* Material::FindConstBuffer(unsigned int& outArrayIndex, unsigned int shaderSlotIndex)
{
	for(unsigned int i=0; i<_constBuffers.size(); ++i)
	{		
		if(_constBuffers[i].semanticIndex == shaderSlotIndex)
		{
			outArrayIndex = i;
			return dynamic_cast<const Buffer::ConstBuffer*>(_constBuffers[i].buffer);
		}
	}

	outArrayIndex = 0;
	return nullptr;
}

const ShaderResourceBuffer* Material::FindShaderResourceBuffer(unsigned int& outArrayIndex, unsigned int shaderSlotIndex)
{
	for(unsigned int i=0; i<_srBuffers.size(); ++i)
	{		
		if(_srBuffers[i].semanticIndex == shaderSlotIndex)
		{
			outArrayIndex = i;
			return dynamic_cast<const ShaderResourceBuffer*>(_srBuffers[i].srBuffer);
		}
	}

	outArrayIndex = 0;
	return nullptr;
}

bool Material::SetShaderResourceBufferUseShaderSlotIndex(unsigned int shaderSlotIndex, const Rendering::Buffer::ShaderResourceBuffer* srBuffer, BaseShader::Usage usage)
{
	unsigned int arrayIdx = 0;
	auto has = FindConstBuffer(arrayIdx, shaderSlotIndex);

	if(has)
	{
		_srBuffers[arrayIdx].srBuffer = srBuffer;
	}
	else
	{
		_srBuffers.push_back(BaseShader::ShaderResourceType(shaderSlotIndex, srBuffer, usage));
	}

	return has != nullptr;
}

bool Material::SetShaderResourceBufferUseArrayIndex(unsigned int arrayIndex, const Rendering::Buffer::ShaderResourceBuffer* srBuffer, BaseShader::Usage usage)
{
	if(arrayIndex >= _srBuffers.size())
		return false;

	_srBuffers[arrayIndex].srBuffer = srBuffer;
	_srBuffers[arrayIndex].SetUsage(usage);

	return true;
}