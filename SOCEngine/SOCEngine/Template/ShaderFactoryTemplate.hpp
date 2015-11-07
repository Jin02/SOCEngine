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
			Rendering::Manager::ShaderManager	*_shaderMgr;

		public:
			[ClassName](Rendering::Manager::ShaderManager* shaderManager)
			{
				_shaderMgr = shaderManager;
			}

			~[ClassName](void)
			{
			}

		public:
			LoadShaderResult LoadShader(const std::string& shaderName,
				const std::string& mainVSFuncName, const std::string& mainPSFuncName, const std::string& mainGSFuncName,
				const std::vector<Shader::ShaderMacro>* macros,
				Shader::VertexShader**		outVertexShader,
				Shader::PixelShader**		outPixelShader,
				Shader::GeometryShader**	outGeometryShader)
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
				
				const std::string baseCommand = shaderName+':';

				Shader::VertexShader* vs = nullptr;
				if(mainVSFuncName.empty() == false)
					vs = _shaderMgr->LoadVertexShader(folderPath, baseCommand + mainVSFuncName, true, vertexDeclations, macros);

				Shader::PixelShader* ps = nullptr;
				if(mainPSFuncName.empty() == false)
					ps = _shaderMgr->LoadPixelShader(folderPath, baseCommand + mainPSFuncName, true, macros);

				Shader::GeometryShader* gs = nullptr;
				if(mainPSFuncName.empty() == false)
					gs = _shaderMgr->LoadGeometryShader(folderPath, baseCommand + mainGSFuncName, true, macros);

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