#pragma once

#include "Common.h"
#include <d3d11.h>
#include <string>

namespace Rendering
{
	namespace Shader
	{
		class ShaderMacro final
		{
		public:
			ShaderMacro() = default;
			ShaderMacro(const std::string& name, const std::string& definition = "") 
				: _name(name), _definition(definition) {}

			GET_CONST_ACCESSOR(Name,		const auto&,		_name);
			SET_ACCESSOR(Name,				const std::string&,	_name);

			GET_CONST_ACCESSOR(Definition,	const auto&,		_definition);
			SET_ACCESSOR(Definition,		const std::string&,	_name);

		private:
			std::string		_name		= "";
			std::string		_definition	= "";
		};
	}
}
