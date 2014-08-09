#include "Material.h"

using namespace Rendering::Material;

Material::Material(const std::string& name, Rendering::Shader::VertexShader* vertexShader, Rendering::Shader::PixelShader* pixelShader) 
 : _vertexShader(vertexShader), _pixelShader(pixelShader), _name(name),
 _mainColor(1.0f, 1.0f, 1.0f, 1.0f)
{

}

Material::~Material(void)
{
}

bool Material::UpdateTextures(unsigned int index, const Rendering::Texture::Texture* texture)
{
	if( index >= _textures.size() )
		return false;

	_textures[index] = texture;

}