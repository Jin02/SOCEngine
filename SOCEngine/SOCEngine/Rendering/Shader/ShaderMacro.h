#pragma once

#include "Common.h"
#include <d3d11.h>
#include <string>

namespace Rendering
{
	namespace Shader
	{
		class ShaderMacro
		{
		public:
			ShaderMacro();
			ShaderMacro(const std::string& name, const std::string& definition);

		public:
			GET_SET_ACCESSOR(Name,			const std::string&,	_name);
			GET_SET_ACCESSOR(Definition,	const std::string&, _definition);

		private:
			std::string		_name;
			std::string		_definition;
		};
	}
}