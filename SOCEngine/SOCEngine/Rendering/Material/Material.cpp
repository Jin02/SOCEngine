#include "Material.h"
#include "Utility.h"
#include "Director.h"

using namespace Rendering;
using namespace Rendering::Buffer;

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
		if(_textures[i].first == shaderSlotIndex)
		{
			outArrayIndex = i;
			return _textures[i].second;
		}
	}

	DEBUG_LOG("Material FindTexture Warning : Undefined UsageTextureType");
	outArrayIndex = 0;
	return nullptr;
}

bool Material::UpdateTexture_ShaderSlotIndex(unsigned int shaderSlotIndex, const Rendering::Texture::Texture2D* texture)
{
	unsigned int arrayIdx = 0;
	auto hasTexture = FindTexture(arrayIdx, shaderSlotIndex);

	if(hasTexture)
	{
		_textures[arrayIdx].second = texture;
	}
	else
	{
		_textures.push_back(std::make_pair(shaderSlotIndex, texture));
	}

	return hasTexture != nullptr;
}

bool Material::UpdateTexture_ArrayIndex(unsigned int arrayIndex, const Rendering::Texture::Texture2D* texture)
{
	if(arrayIndex >= _textures.size())
		return false;

	_textures[arrayIndex].second = texture;
	return true;
}

bool Material::UpdateConstBuffer_ShaderSlotIndex(uint shaderSlotIdx, const Buffer::ConstBuffer* cb)
{
	unsigned int arrayIdx = 0;
	auto has = FindConstBuffer(arrayIdx, shaderSlotIdx);

	if(has)
	{
		_constBuffers[arrayIdx].second = cb;
	}
	else
	{
		_constBuffers.push_back(std::make_pair(shaderSlotIdx, cb));
	}

	return has != nullptr;
}

bool Material::UpdateConstBuffer_ArrayIndex(uint arrayIdx, const Buffer::ConstBuffer* cb)
{
	if(arrayIdx >= _constBuffers.size())
		return false;

	_constBuffers[arrayIdx].second = cb;
	return true;
}

const Buffer::ConstBuffer* Material::FindConstBuffer(unsigned int& outArrayIndex, unsigned int shaderSlotIndex)
{
	for(unsigned int i=0; i<_constBuffers.size(); ++i)
	{		
		if(_constBuffers[i].first == shaderSlotIndex)
		{
			outArrayIndex = i;
			return dynamic_cast<const Buffer::ConstBuffer*>(_constBuffers[i].second);
		}
	}

	outArrayIndex = 0;
	return nullptr;
}