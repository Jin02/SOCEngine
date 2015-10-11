#pragma once

#include "BaseBuffer.h"

namespace Rendering
{
	namespace Buffer
	{
		class IndexBuffer : public BaseBuffer
		{		
		private:
			uint			_indexCount;
			std::string		_useVertexBufferKey;

		public:
			IndexBuffer();
			virtual ~IndexBuffer();

		public:
			bool Initialize(const std::vector<uint>& indices, const std::string& useVertexBufferKey, bool isDynamic = false);
			void IASetBuffer(ID3D11DeviceContext* context);

		public:
			GET_ACCESSOR(IndexCount, uint, _indexCount);
			GET_ACCESSOR(UseVertexBufferKey, const std::string&, _useVertexBufferKey);
		};
	}
}