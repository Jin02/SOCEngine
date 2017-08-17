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

			void Initialize(Device::DirectX& dx, const std::vector<uint>& indices, uint vbChunkKey, bool isDynamic = false);
			void IASetBuffer(Device::DirectX& dx);

			GET_CONST_ACCESSOR(IndexCount, uint, _indexCount);
			GET_CONST_ACCESSOR(VBChunkKey, uint, _vbChunkKey);

		private:
			uint		_indexCount		= 0;
			uint		_vbChunkKey		= -1;
			BaseBuffer	_baseBuffer;
		};
	}
}
