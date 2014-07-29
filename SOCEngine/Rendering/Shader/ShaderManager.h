#pragma once

#include <string>
#include <fstream>
#include "HashMap.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "Utility.h"

namespace Rendering
{
	namespace Shader
	{
		class ShaderManager
		{
		private:
			Structure::HashMap<Shader> _shaders;
			Structure::HashMap<ShaderCode> _shaderCodes;

		public:
			ShaderManager()
			{
			}

			~ShaderManager(void)
			{
			}

		public:
			bool Test(const std::string& folderPath, const std::string& command, bool recycle)
			{
				std::vector<std::string> commands;
				Utility::Tokenize(command, commands, ":");

				if(commands.size() != 3)
					return false;

				std::string fileName	= commands[0];
				std::string shaderType	= commands[1];
				std::string mainFunc	= commands[2];

				std::string code;

				if(recycle)
				{
					std::ifstream file;
					const char* extension[2] = {".fx", ".hlsl"};
					for(int i=0; i<2; ++i)
					{
						file.open(folderPath+fileName+extension[i]);

						if(file.is_open())
							break;
					}

					if(file.good() == false)
					{
						file.close();
						return false;
					}

					std::string buff;

					while(std::getline(file, buff))
					{
						code += buff;
						code += "\n";
					}
				}

				ID3DBlob* blob;
				Rendering::Shader::Shader::CompileFromMemory(&blob, code, shaderType+"_5_0", mainFunc);

				return true;
			}
		};

	}
}