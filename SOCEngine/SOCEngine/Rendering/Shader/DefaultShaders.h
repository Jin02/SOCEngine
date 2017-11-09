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
		class DefaultShaders
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
			bool			Has(uint key) const;
			const Shaders&	Find(uint key) const;
			const Shaders&	Find(uint bufferFlag, DefaultRenderType renderType) const;

			static uint MakeKey(uint bufferFlag, DefaultRenderType renderType);
			std::string MakeDefaultSahderFileName(DefaultRenderType renderType, uint bufferFlag) const;

		private:
			void Initialize(Device::DirectX& dx, ShaderManager& shaderMgr);
			const Shaders&	Add(uint key, const Shader::ShaderGroup& shaders);
			void Destroy();

			const Shaders& LoadDefaultSahder(	Device::DirectX& dx, ShaderManager& shaderMgr,
												DefaultRenderType renderType, uint bufferFlag, 
												const std::vector<Shader::ShaderMacro>* macros);
			void MakeDefaultShaderMainFuncNames(std::string& outVSMain, std::string& outGSMain, std::string& outPSMain, DefaultRenderType renderType);

		private:
			std::unordered_map<uint, Shaders> _shaders; 
		};
	}
}