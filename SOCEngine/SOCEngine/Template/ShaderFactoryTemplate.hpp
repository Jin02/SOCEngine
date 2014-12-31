#pragma once

#include <ShaderManager.h>

namespace Rendering
{
	namespace Factory
	{
		class [ClassName]
		{
		private:
			Rendering::Manager::ShaderManager	*_shaderMgr;

		public:
			[ClassName](Rendering::Manager::ShaderManager*& shaderManager)
			{
				_shaderMgr = shaderManager;
			}

			~[ClassName](void)
			{
			}

		public:
			bool LoadShader(const std::string& shaderName,
				const std::string& mainVSFuncName, const std::string& mainPSFuncName,
				const std::string* includeFileName,
				Shader::VertexShader** outVertexShader,
				Shader::PixelShader** outPixelShader)
			{
				std::string folderPath = "";
				std::vector<D3D11_INPUT_ELEMENT_DESC> vertexDeclations;

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

				Shader::VertexShader* vs = _shaderMgr->LoadVertexShader(folderPath, baseCommand + mainVSFuncName, true, vertexDeclations, includeFileName);
				Shader::PixelShader* ps = _shaderMgr->LoadPixelShader(folderPath, baseCommand + mainPSFuncName, true, includeFileName);

				if(outVertexShader)
					(*outVertexShader) = vs;
	
				if(outPixelShader)
					(*outPixelShader) = ps;

				return (vs && ps);
			}
		};
	}
}