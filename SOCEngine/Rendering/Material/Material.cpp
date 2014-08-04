#include "Material.h"

using namespace Rendering::Material;

Material::Material(const std::string& name, Rendering::Shader::VertexShader* vertexShader, Rendering::Shader::PixelShader* pixelShader) 
 : _vertexShader(vertexShader), _pixelShader(pixelShader), _name(name)
{
}

Material::~Material(void)
{
}