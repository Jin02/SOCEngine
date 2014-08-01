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

		public:
			Material() : _vertexShader(nullptr), _pixelShader(nullptr)
			{
			}

			~Material(void)
			{
			}

		public:
			void Begin()
			{
				_vertexShader->Begin();
				_pixelShader->Begin();
			}

			//꼭 호출이 필수는 아님.
			void End()
			{
				_vertexShader->End();
				_pixelShader->End();
			}

		public:
			GET_SET_ACCESSOR(VertexShader, Shader::VertexShader*, _vertexShader);
			GET_SET_ACCESSOR(PixelShader, Shader::PixelShader*, _pixelShader);
		};

	}
}