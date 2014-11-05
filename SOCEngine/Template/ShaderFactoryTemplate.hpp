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
				Rendering::Shader::VertexShader*& outVertexShader,
				Rendering::Shader::PixelShader*& outPixelShader)
			{
				std::string folderPath = "";
				std::vector<D3D11_INPUT_ELEMENT_DESC> vertexDeclations;
				D3D11_INPUT_ELEMENT_DESC desc;
				unsigned int vsFlag = 0;

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

					if(semanticName == "POSITION")
						vsFlag |= Shader::VertexShader::Flag::POSITION;
					else if(semanticName == "NORMAL")
						vsFlag |= Shader::VertexShader::Flag::NORMAL;
					else if(semanticName == "BINORMAL")
						vsFlag |= Shader::VertexShader::Flag::BINORMAL;
					else if(semanticName == "TANGENT")
						vsFlag |= Shader::VertexShader::Flag::TANGENT;
					else if(semanticName == "TEXCOORD")
						vsFlag |= (Shader::VertexShader::Flag::TEXCOORD0 << semanticIndex);
					//else if(semanticName.find("INSTANCE") != -1)
					//{
					//	//추후 처리
					//}
				};
				/** Script Begin **/
				/** Script End **/
				const std::string baseCommand = shaderName+":";
				outVertexShader = dynamic_cast<Rendering::Shader::VertexShader*>(_shaderMgr->LoadVertexShader(folderPath, baseCommand + mainVSFuncName, true, vertexDeclations));
				outPixelShader	= dynamic_cast<Rendering::Shader::PixelShader*>(_shaderMgr->LoadPixelShader(folderPath, baseCommand + mainPSFuncName, false));
				return (outVertexShader && outPixelShader);
			}
		};
	}
}