#pragma once

#include "BaseBuffer.h"

namespace Rendering
{
	namespace Buffer
	{
		class IndexBuffer final
		{		
		public:
			IndexBuffer() = default;

			void Initialize(Device::DirectX& dx, const std::vector<uint>& indices, const std::string& useVertexBufferKey, bool isDynamic = false);
			void IASetBuffer(Device::DirectX& dx);

			GET_CONST_ACCESSOR(IndexCount, uint, _indexCount);
			GET_CONST_ACCESSOR(UseVertexBufferKey, const std::string&, _useVertexBufferKey);

		private:
			uint			_indexCount		= 0;
			std::string		_useVertexBufferKey	= "";
			BaseBuffer		_baseBuffer;
		};
	}
}
