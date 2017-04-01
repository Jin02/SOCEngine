#pragma once

#include "DirectX.h"
#include "BufferManager.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

namespace Rendering
{
	namespace Sky
	{
		class BaseSky final
		{
		public:
			BaseSky() = default;
			void Initialize(Device::DirectX& dx, Manager::BufferManager& bufferMgr);

		private:
			Buffer::VertexBuffer		_vertexBuffer;
			Buffer::IndexBuffer			_indexBuffer;
		};
	}
}