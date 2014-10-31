#include "Material.h"

using namespace Rendering::Material;

Material::Color::Color() 
	:diffuse(1.0f, 1.0f, 1.0f), ambient(0.5f, 0.5f, 0.5f),
	specular(1.0f, 1.0f, 1.0f), emissive(0.0f, 0.0f, 0.0f),
	shiness(0.0f), opacity(1.0f)
{
}

Material::Color::~Color()
{
}


Material::Material(const std::string& name)	
	: _vertexShader(nullptr), _pixelShader(nullptr), _name(name)
{

}

Material::Material(const std::string& name, const Color& color) 
	: _vertexShader(nullptr), _pixelShader(nullptr), _name(name), _color(color)
{

}

Material::~Material(void)
{
}

bool Material::UpdateTexture(unsigned int index, const Rendering::Texture::Texture* texture)
{	
	if( index >= _textures.size() )
		_textures.resize(index+1);

	_textures[index] = texture;

	return true;
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

void Material::UpdateAmbientMap(const Texture::Texture* tex)
{
	if(UpdateTexture(TextureType::Ambient, tex) == false)
		_textures.push_back(tex);
}