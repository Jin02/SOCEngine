#pragma once

#include <ShaderManager.h>

namespace Rendering
{
	namespace Shader
	{
		class Factory
		{
		private:
			Rendering::Shader::ShaderManager	*_shaderMgr;

		public:
			Factory(Rendering::Shader::ShaderManager*& shaderManager)
			{
				_shaderMgr = shaderManager;
			}

			~Factory(void)
			{
			}

		public:
			bool LoadShader(const std::string& shaderName,
				const std::string& mainVSFuncName, const std::string& mainPSFuncName,
				Rendering::Shader::VertexShader** outVertexShader,
				Rendering::Shader::PixelShader** outPixelShader)
			{
				std::string folderPath = "";
				std::vector<D3D11_INPUT_ELEMENT_DESC> vertexDeclations;

				typedef unsigned int uint;
				auto AddInputElementDesc = [&](const std::string& semanticName, uint semanticIndex, DXGI_FORMAT format, uint alignedByteOffset, D3D11_INPUT_CLASSIFICATION inputSlotClass, uint inputSlot, uint instanceDataStepRate)
				{
					auto MakeInputElementDesc = [&](D3D11_INPUT_ELEMENT_DESC& out)
					{
						out.SemanticName = semanticName.c_str();
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

				VertexShader* vs = _shaderMgr->LoadVertexShader(folderPath, baseCommand + mainVSFuncName, true, vertexDeclations);
				PixelShader* ps = _shaderMgr->LoadPixelShader(folderPath, baseCommand + mainPSFuncName, true);

				if(outVertexShader)
					(*outVertexShader) = vs;
	
				if(outPixelShader)
					(*outPixelShader) = ps;

				return (outVertexShader && outPixelShader);
			}
		};
	}
}