#pragma once

#include "BaseShader.h"
#include "ConstBuffer.h"

namespace Rendering
{
	namespace Shader
	{
		class VertexShader : public BaseShader
		{
		public:
			struct SemanticInfo
			{
				std::string name;
				unsigned int size;
			};

		private:
			std::vector<SemanticInfo> _semanticInfo;

		private:
			ID3D11VertexShader* _shader;
			ID3D11InputLayout*	_layout;

		public:
			VertexShader(ID3DBlob* blob);
			~VertexShader(void);

		public:
			GET_ACCESSOR(SemanticInfos, const std::vector<SemanticInfo>&, _semanticInfo);

		public:
			bool CreateShader(const D3D11_INPUT_ELEMENT_DESC* vertexDeclations, unsigned int count);
			void ClearResource(ID3D11DeviceContext* context, const std::vector<TextureType>* textures);

		public:
			void UpdateShader(ID3D11DeviceContext* context);
			void UpdateResources(ID3D11DeviceContext* context, const std::vector<BufferType>* constBuffers, const std::vector<TextureType>* textures);
		};
	}
}
