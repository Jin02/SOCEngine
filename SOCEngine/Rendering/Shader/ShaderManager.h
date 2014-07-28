#pragma once

#include "SOCHashMap.h"
#include <string>
#include <fstream>
#include "ShaderCode.h"
#include "Graphics.h"
#include "Shader.h"

namespace Rendering
{
	namespace Shader
	{
		class ShaderManager
		{
		private:
			Device::Graphics *graphics;
			SOCHashMap<std::string, std::pair<ShaderCode, Shader*>> hash;

		private:
			const char *shaderDir;
			int dirLen;

		public:
			ShaderManager(Device::Graphics *graphics);
			~ShaderManager(void);

		private:
			bool LoadShaderFromFile( std::string &path, std::pair<ShaderCode, Shader*> *outPair, bool shaderClone, bool inShaderFolder = true);
			bool FindShader(std::string &path, std::pair<ShaderCode, Shader*> *outPair, bool shaderClone);

		public:
			bool LoadShaderFromFile( std::string path, ShaderCode *outShaderCode, bool inShaderFolder = true);
			bool FindShader(std::string path, ShaderCode *outShaderCode);

			bool LoadShaderFromFile( std::string path, Shader **outShader, bool shaderClone, bool inShaderFolder = true);
			bool FindShader(std::string path, Shader **outShader, bool shaderClone);

			void DeleteAll();
		};

	}
}