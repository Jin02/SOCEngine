#pragma once

#include "BaseBuffer.h"

namespace Rendering
{
	namespace Buffer
	{
		class VertexBuffer : public BaseBuffer
		{
		public:
			struct SemanticInfo //Attributes
			{
				std::string name;
				uint size;

				SemanticInfo() : size(0), name("") {}
				~SemanticInfo() {}
			};

		private:
			unsigned int				_stride;
			std::vector<SemanticInfo>	_semantics; //attributes
			uint						_vertexCount;

		public:
			VertexBuffer();
			virtual ~VertexBuffer();

		public:
			void Initialize(const void* sysMem, unsigned int bufferStrideSize,
				unsigned int count, bool isDynamic, const std::vector<SemanticInfo>* semanticInfos);
			void IASetBuffer(ID3D11DeviceContext* context);
			void UpdateVertexData(ID3D11DeviceContext* context, const void* data, uint size);

		public:
			GET_ACCESSOR(VertexCount, uint, _vertexCount);
			GET_ACCESSOR(Semantics, const std::vector<SemanticInfo>&, _semantics);
		};
	}
}