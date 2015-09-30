#pragma once

#include "BaseBuffer.h"
#include "VertexShader.h"

namespace Rendering
{
	namespace Buffer
	{
		class VertexBuffer : public BaseBuffer
		{
		private:
			std::string					_key;
			unsigned int				_stride;
			uint						_vertexCount;

			std::vector<Shader::VertexShader::SemanticInfo>	_semantics; //attributes

		public:
			VertexBuffer();
			virtual ~VertexBuffer();

		public:
			void Initialize(const void* sysMem, unsigned int bufferStrideSize,
				unsigned int count, bool isDynamic, const std::string& key,
				const std::vector<Shader::VertexShader::SemanticInfo>* semanticInfos);
			void IASetBuffer(ID3D11DeviceContext* context);
			void UpdateVertexData(ID3D11DeviceContext* context, const void* data, uint size);

		public:
			GET_ACCESSOR(VertexCount, uint, _vertexCount);
			GET_ACCESSOR(Key, const std::string&, _key);

			GET_ACCESSOR(Semantics, const std::vector<Shader::VertexShader::SemanticInfo>&, _semantics);
		};
	}
}