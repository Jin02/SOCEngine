#pragma once

#include "Common.h"
#include "DefaultRenderTypes.h"
#include "Shaders.h"
#include "ShaderManager.h"
#include <string>
#include <unordered_map>

namespace Rendering
{
	namespace Manager
	{
		class DefaultShaderLoader
		{
		public:
			struct Shaders
			{
				Shader::VertexShader	vs;
				Shader::PixelShader		ps;
				Shader::GeometryShader	gs;

				Shaders() = default;
				Shaders(const Shader::ShaderGroup& shaderGroup);
			};

		public:
			void Initialize(Device::DirectX& dx, ShaderManager& shaderMgr);
			void Destroy();

			Shaders& Add(uint key, const Shader::ShaderGroup& shaders);
			bool Has(uint key) const;
			Shaders& Find(uint key);

			static uint MakeKey(uint bufferFlag, DefaultRenderType renderType);
			std::string MakeDefaultSahderFileName(DefaultRenderType renderType, uint bufferFlag) const;

		private:
			Shaders& LoadDefaultSahder(Device::DirectX& dx, ShaderManager& shaderMgr,
				DefaultRenderType renderType, uint bufferFlag, const std::vector<Shader::ShaderMacro>* macros);
			void MakeDefaultShaderMainFuncNames(std::string& outVSMain, std::string& outGSMain, std::string& outPSMain, DefaultRenderType renderType);

		private:
			std::unordered_map<uint, Shaders> _shaders; 
		};
	}
}