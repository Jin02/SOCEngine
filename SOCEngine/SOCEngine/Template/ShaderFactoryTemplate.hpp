#pragma once

#include "ShaderManager.h"
#include "ShaderCompiler.h"
#include "Utility.hpp"

namespace Rendering
{
	namespace Factory
	{
		class [ClassName]
		{
		public:
			struct LoadShaderResult
			{
				bool loadVS;
				bool loadPS;
				bool loadGS;
				bool isOnlyHasPath;
			};

		private:
			Manager::ShaderManager*	_shaderMgr;
			Shader::ShaderCompiler*	_compiler;

		public:
			[ClassName](Manager::ShaderManager* shaderMgr) : _shaderMgr(shaderMgr)
			{
			}

		public:
			LoadShaderResult LoadShader(
				Device::DirectX& dx,
				const std::string& shaderName,
				const std::string& mainVSFuncName, const std::string& mainPSFuncName, const std::string& mainGSFuncName,
				const std::vector<Shader::ShaderMacro>* macros,
				std::shared_ptr<Shader::VertexShader>*		outVertexShader,
				std::shared_ptr<Shader::PixelShader>*		outPixelShader,
				std::shared_ptr<Shader::GeometryShader>*	outGeometryShader)
			{
				std::string folderPath = "";
				std::vector<D3D11_INPUT_ELEMENT_DESC> vertexDeclations;
				bool isOnlyHasPath = false;

				typedef unsigned int uint;
				auto AddInputElementDesc = [&](const char* semanticName, uint semanticIndex, DXGI_FORMAT format, uint alignedByteOffset, D3D11_INPUT_CLASSIFICATION inputSlotClass, uint inputSlot, uint instanceDataStepRate)
				{
					auto MakeInputElementDesc = [&](D3D11_INPUT_ELEMENT_DESC& out)
					{
						out.SemanticName = semanticName;
						out.SemanticIndex = semanticIndex;
						out.AlignedByteOffset = alignedByteOffset;
						out.Format = format;
						out.InputSlotClass = inputSlotClass;
						out.InputSlot = inputSlot;
						out.InstanceDataStepRate = instanceDataStepRate;
					};

					D3D11_INPUT_ELEMENT_DESC desc;
					MakeInputElementDesc(desc);
					vertexDeclations.push_back(desc);
				};

				/** Script Begin **/
				/** Script End **/
				
				assert(_shaderMgr);
				auto& compiler = _shaderMgr->GetCompiler();

				std::shared_ptr<Shader::VertexShader> vs = nullptr;
				if (mainVSFuncName.empty() == false)
				{
					auto blob = compiler.CreateBlob(folderPath, shaderName, "vs", mainVSFuncName, true, macros);
					std::string key = Shader::ShaderCompiler::MakeKey(shaderName, mainVSFuncName, "vs", macros);

					vs = std::make_shared<Shader::VertexShader>(blob, key);
					vs->Initialize(dx, vertexDeclations);

					_shaderMgr->GetPool<Shader::VertexShader>().Add(key, vs);
				}

				std::shared_ptr<Shader::PixelShader> ps = nullptr;
				if (mainPSFuncName.empty() == false)
				{
					auto blob = compiler.CreateBlob(folderPath, shaderName, "ps", mainPSFuncName, true, macros);
					std::string key = Shader::ShaderCompiler::MakeKey(shaderName, mainPSFuncName, "ps", macros);

					ps = std::make_shared<Shader::PixelShader>(blob, key);
					ps->Initialize(dx);

					_shaderMgr->GetPool<Shader::PixelShader>().Add(key, ps);
				}

				std::shared_ptr<Shader::GeometryShader> gs = nullptr;
				if (mainGSFuncName.empty() == false)
				{
					auto blob = compiler.CreateBlob(folderPath, shaderName, "gs", mainGSFuncName, true, macros);
					std::string key = Shader::ShaderCompiler::MakeKey(shaderName, mainGSFuncName, "gs", macros);

					gs = std::make_shared<Shader::GeometryShader>(blob, key);
					gs->Initialize(dx);

					_shaderMgr->GetPool<Shader::GeometryShader>().Add(key, gs);
				}

				if(outVertexShader)
					(*outVertexShader) = vs;
	
				if(outPixelShader)
					(*outPixelShader) = ps;

				if(outGeometryShader)
					(*outGeometryShader) = gs;

				LoadShaderResult result;
				result.loadVS = vs != nullptr;
				result.loadPS = ps != nullptr;
				result.loadGS = gs != nullptr;
				result.isOnlyHasPath = isOnlyHasPath;

				return result;
			}
			
			void FetchShaderFullPath(std::string& out, const std::string& fileName)
			{
				/** FullPath Script Begin **/
				/** FullPath Script End **/
			}

			std::shared_ptr<Shader::ComputeShader> LoadComputeShader(
				Device::DirectX& dx,
				const std::string& shaderName, const std::string& mainFuncName,
				const std::vector<Shader::ShaderMacro>* macros,
				const std::string& uniqueKey)
			{
				assert(_shaderMgr);
				auto& compiler = _shaderMgr->GetCompiler();

				std::string fullPath = "";
				FetchShaderFullPath(fullPath, shaderName);

				std::string folderPath = "";
				assert(Utility::String::ParseDirectory(fullPath, &folderPath, nullptr, nullptr));

				auto blob = compiler.CreateBlob(folderPath, shaderName, "cs", mainFuncName, true, macros);

				std::string key = (uniqueKey.empty() == false) ? uniqueKey : Shader::ShaderCompiler::MakeKey(shaderName, mainFuncName, "cs", macros);
				auto shader = std::make_shared<Shader::ComputeShader>(blob, key);
				shader->Initialize(dx);

				_shaderMgr->GetPool<Shader::ComputeShader>().Add(key, shader);

				return shader;
			}
		};
	}
}