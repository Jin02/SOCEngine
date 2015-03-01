#include "Material.h"
#include "Utility.h"
#include "Director.h"

using namespace Rendering;
using namespace Rendering::Buffer;

Material::Material(const std::string& name)	
	: _name(name), _hasAlpha(false), _changedAlpha(true),
	  _tiling(1.0f, 1.0f)
{

}

Material::~Material(void)
{

}

void Material::Init(const std::vector<unsigned int>& textureShaderSlotIndex)
{
	for(auto& idx : textureShaderSlotIndex)
		_textures.push_back( std::make_pair(idx, nullptr) );
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

	DEBUG_LOG("Material FindTexture Error : Undefined UsageTextureType");
	outArrayIndex = 0;
	return nullptr;
}

bool Material::UpdateTextureUseShaderSlotIndex(unsigned int shaderSlotIndex, const Rendering::Texture::Texture* texture)
{
	unsigned int arrayIdx = 0;
	auto hasTexture = FindTexture(arrayIdx, shaderSlotIndex);

	if(hasTexture)
		_textures[arrayIdx].second = texture;

	return hasTexture != nullptr;
}

bool Material::UpdateTextureUseArrayIndex(unsigned int arrayIndex, const Rendering::Texture::Texture* texture)
{
	if(arrayIndex >= _textures.size())
		return false;

	_textures[arrayIndex].second = texture;
	return true;
}