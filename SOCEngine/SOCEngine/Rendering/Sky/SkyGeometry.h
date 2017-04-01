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
			SkyGeometry() = default;
			void Initialize(Device::DirectX& dx, Manager::BufferManager& bufferMgr);

		private:
			Buffer::VertexBuffer		_vertexBuffer;
			Buffer::IndexBuffer			_indexBuffer;
		};
	}
}