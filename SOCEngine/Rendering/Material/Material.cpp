#include "Material.h"

using namespace Rendering::Material;

Material::LightColor::LightColor() 
	:diffuse(1.0f, 1.0f, 1.0f), ambient(0.5f, 0.5f, 0.5f),
	specular(1.0f, 1.0f, 1.0f), emissive(0.0f, 0.0f, 0.0f),
	specularExponent(0.0f)
{
}

Material::LightColor::~LightColor()
{
}


Material::Material(const std::string& name)	
	: _vertexShader(nullptr), _pixelShader(nullptr), _name(name)
{

}

Material::Material(const std::string& name, const LightColor& lightColor) 
	: _vertexShader(nullptr), _pixelShader(nullptr), _name(name), _lightColor(lightColor)
{

}

Material::~Material(void)
{
}

bool Material::UpdateTexture(unsigned int index, const Rendering::Texture::Texture* texture)
{
	if( index >= _textures.size() )
		return false;

	_textures[index] = texture;
}

void Material::UpdateDiffuseMap(const Rendering::Texture::Texture* tex)
{
	if(UpdateTexture(TextureType::Diffuse, tex) == false)
		_textures.push_back(tex);
}

void Material::UpdateNormalMap(const Rendering::Texture::Texture* tex)
{
	if(UpdateTexture(TextureType::Normal, tex) == false)
		_textures.push_back(tex);
}

void Material::UpdateSpecularMap(const Rendering::Texture::Texture* tex)
{
	if(UpdateTexture(TextureType::Specular, tex) == false)
		_textures.push_back(tex);
}

void Material::UpdateOpacityMap(const Rendering::Texture::Texture* tex)
{
	if(UpdateTexture(TextureType::Opacity, tex) == false)
		_textures.push_back(tex);
}