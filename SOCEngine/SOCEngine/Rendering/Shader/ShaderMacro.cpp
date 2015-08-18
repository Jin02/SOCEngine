#include "ShaderMacro.h"

using namespace Rendering::Shader;

ShaderMacro::ShaderMacro() : _name(""), _definition("")
{
}

ShaderMacro::ShaderMacro(const std::string& name, const std::string& definition)
	: _name(name), _definition(definition)
{
}

ShaderMacro::~ShaderMacro()
{
}