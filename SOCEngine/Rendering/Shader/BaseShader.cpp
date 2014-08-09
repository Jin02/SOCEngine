#include "BaseShader.h"

using namespace Rendering::Shader;

BaseShader::BaseShader(ID3DBlob* blob) : _blob(blob), _type(Type::Invalid)
{
}

BaseShader::~BaseShader(void)
{
}