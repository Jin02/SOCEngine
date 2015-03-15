#include "Material.h"
#include "Utility.h"
#include "Director.h"

using namespace Rendering;
using namespace Rendering::Buffer;

Material::Material(const std::string& name, Type type)	
	: _name(name), _hasAlpha(false), _changedAlpha(true),
	_tiling(1.0f, 1.0f), _type(type)
{
}

Material::~Material(void)
{

}

const Rendering::Texture::Texture* Material::FindTexture(unsigned int& outArrayIndex, unsigned int shaderSlotIndex)
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

bool Material::UpdateTextureUseShaderSlotIndex(unsigned int shaderSlotIndex, const Rendering::Texture::Texture* texture)
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

bool Material::UpdateTextureUseArrayIndex(unsigned int arrayIndex, const Rendering::Texture::Texture* texture)
{
	if(arrayIndex >= _textures.size())
		return false;

	_textures[arrayIndex].second = texture;
	return true;
}