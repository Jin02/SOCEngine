#pragma once

#include "BaseBuffer.h"
#include "Utility.hpp"

namespace Rendering
{
	namespace Buffer
	{
		class VertexBuffer;
		class IndexBuffer final
		{		
		public:
			IndexBuffer() = default;

			void Initialize(Device::DirectX& dx, const std::vector<uint>& indices, BaseBuffer::Key vbKey, bool isDynamic = false);
			void IASetBuffer(Device::DirectX& dx) const;

			GET_CONST_ACCESSOR(IndexCount,	uint,				_indexCount);
			GET_CONST_ACCESSOR(VBKey,		BaseBuffer::Key,	_vbKey);

		private:
			uint					_indexCount		= 0;
			BaseBuffer::Key			_vbKey;
			BaseBuffer				_baseBuffer;
		};
	}
}
