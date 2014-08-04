#pragma once

#include "VertexShader.h"
#include "PixelShader.h"
#include "Common.h"

namespace Rendering
{
	namespace Material
	{
		class Material
		{
		private:
			Shader::VertexShader*	_vertexShader;
			Shader::PixelShader*	_pixelShader;
			std::string	_name;

		public:
			Material(const std::string& name, Shader::VertexShader* vertexShader, Shader::PixelShader* pixelShader);
			~Material(void);

		public:
			GET_ACCESSOR(VertexShader, Shader::VertexShader*, _vertexShader);
			GET_ACCESSOR(PixelShader, Shader::PixelShader*, _pixelShader);
			GET_ACCESSOR(Name, const std::string&, _name);
		};

	}
}