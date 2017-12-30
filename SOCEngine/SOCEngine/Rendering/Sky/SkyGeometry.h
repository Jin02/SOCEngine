#pragma once

#include "DirectX.h"
#include "BufferManager.hpp"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

namespace Rendering
{
	namespace Sky
	{
		class SkyGeometry final
		{
		public:
			void Initialize(Device::DirectX& dx, Manager::BufferManager& bufferMgr);
			void Destroy();

			GET_CONST_ACCESSOR(VertexBuffer,	const auto&, _vertexBuffer);
			GET_CONST_ACCESSOR(IndexBuffer,		const auto&, _indexBuffer);

		private:
			Buffer::VertexBuffer	_vertexBuffer;
			Buffer::IndexBuffer		_indexBuffer;
		};
	}
}