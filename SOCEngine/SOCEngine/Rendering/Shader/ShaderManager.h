#pragma once

#include <string>
#include <fstream>
#include "VertexShaderManager.h"
#include "ShaderPool.hpp"
#include "PixelShader.h"
#include "GeometryShader.h"
#include "ShaderMacro.hpp"
#include "VectorIndexer.hpp"
#include "ComputeShaderManager.h"
#include "DirectX.h"
#include "ShaderMacro.hpp"
#include <tuple>

namespace Rendering
{
	namespace Manager
	{
		using PixelShaderManager	= ShaderPool<Shader::PixelShader>;
		using GeometryShaderManager	= ShaderPool<Shader::GeometryShader>;

		class ShaderManager final
		{
		public:
			ShaderManager();

			static void				Compile(DXResource<ID3DBlob>* outBlob, const std::string &fileFullPath, const std::string& shaderCode, const std::string& shaderModel, const std::string& funcName, const std::vector<Shader::ShaderMacro>* macros);
			static bool				MakeShaderFileFullPath(std::string& outFullPath, const std::string& folderPath, const std::string& fileName);

			static std::string		MakeKey(const std::string& fileName, const std::string& mainFunc, const std::string& shaderTypeStr, const std::vector<Shader::ShaderMacro>* macros);

			DXResource<ID3DBlob>	CreateBlob(const std::string& folderPath, const std::string& fileName, const std::string& shaderType, const std::string& mainFunc, bool useRecycle, const std::vector<Shader::ShaderMacro>* macros);
			bool					LoadShaderCode(std::string& outCode, const std::string& folderPath, const std::string& fileName, bool useRecycle);

			const char*				FindShaderCode(const std::string& fileName);

			void					DeleteShaderCode(const std::string& key);
			void					Destroy();

			GET_ACCESSOR(VertexShaderManager,	VertexShaderManager&,	_vsMgr);
			GET_ACCESSOR(PixelShaderManager,	PixelShaderManager&,	_psMgr);
			GET_ACCESSOR(GeometryShaderManager,	GeometryShaderManager&,	_gsMgr);
			GET_ACCESSOR(ComputeShaderManager,	ComputeShaderManager&,	_csMgr);

		private:
			VertexShaderManager					_vsMgr;
			PixelShaderManager					_psMgr;
			GeometryShaderManager				_gsMgr;
			ComputeShaderManager				_csMgr;

			std::map<std::string, std::string>	_shaderCodes;
		};
	}
}