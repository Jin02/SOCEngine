#pragma once

#if defined(WIN32) && !defined(_USE_GL_DEFINES)

#include <string>

namespace Rendering
{
	namespace Shader
	{
		typedef std::string ShaderCode;
	}
}

#elif defined(__APPLE__) || defined(_USE_GL_DEFINES)

#endif