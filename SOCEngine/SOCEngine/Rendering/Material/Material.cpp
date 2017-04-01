#include "Material.h"

using namespace Rendering;
using namespace Rendering::Buffer;
using namespace Rendering::Shader;
using namespace Rendering::Texture;

Material::Material(const std::string& name)
	: _constBuffers(), _textures(), _srBuffers(),
	_name(name), _alpha(1.0f)
{
}