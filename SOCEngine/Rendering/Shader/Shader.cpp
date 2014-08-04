#include "Shader.h"

using namespace Rendering::Shader;

Shader::Shader(ID3DBlob* blob) : _blob(blob), _type(Type::Invalid)
{
}

Shader::~Shader(void)
{
}