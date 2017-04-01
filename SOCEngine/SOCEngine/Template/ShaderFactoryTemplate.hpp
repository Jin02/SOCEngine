#pragma once

#include "ShaderManager.h"

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

		public:
			[ClassName](Manager::ShaderManager* shaderMgr)
				: _shaderMgr(shaderMgr)
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

				std::shared_ptr<Shader::VertexShader> vs = nullptr;
				if (mainVSFuncName.empty() == false)
					vs = _shaderMgr->GetVertexShaderManager().LoadShader(dx, folderPath, shaderName, mainVSFuncName, true, vertexDeclations, macros, nullptr);

				std::shared_ptr<Shader::PixelShader> ps = nullptr;
				if (mainPSFuncName.empty() == false)
					ps = _shaderMgr->GetPixelShaderManager().LoadShader(dx, folderPath, shaderName, mainPSFuncName, true, macros, nullptr);

				std::shared_ptr<Shader::GeometryShader> gs = nullptr;
				if (mainPSFuncName.empty() == false)
					gs = _shaderMgr->GetGeometryShaderManager().LoadShader(dx, folderPath, shaderName, mainGSFuncName, true, macros, nullptr);

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
		};
	}
}