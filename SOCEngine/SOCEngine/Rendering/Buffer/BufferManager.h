#pragma once

#include "ConstBuffer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

#include "Structure.h"
#include "LPVoidType.h"

namespace Rendering
{
	namespace Manager
	{
		class BufferManager
		{
		private:
			Structure::MapInMap<std::string, Buffer::VertexBuffer*>		_vertexBuffers;
			Structure::MapInMap<std::string, Buffer::IndexBuffer*>		_indexBuffers;
			Structure::MapInMap<std::string, LPVoidType>				_originVertexBufferDatas;

		public:
			BufferManager();
			~BufferManager();

		public:
			void Add(const std::string& file, const std::string& key, Buffer::VertexBuffer* bufferData);
			void Add(const std::string& file, const std::string& key, Buffer::IndexBuffer* bufferData);
			void Add(const std::string& file, const std::string& key, const LPVoidType& bufferData);

			bool Find(Buffer::VertexBuffer** outBuffer, const std::string& file, const std::string& key);
			bool Find(Buffer::IndexBuffer** outBuffer, const std::string& file, const std::string& key);
			bool Find(LPVoidType* outBuffer, const std::string& file, const std::string& key);

			void DeleteVertexBuffer(const std::string& file, const std::string& key);
			void DeleteIndexBuffer(const std::string& file, const std::string& key);
			void DeleteOriginVBData(const std::string& file, const std::string& key);

			void DeleteVertexBuffer(const std::string& file);
			void DeleteIndexBuffer(const std::string& file);
			void DeleteOriginVBData(const std::string& file);

			void DeleteAllVertexBuffer();
			void DeleteAllIndexBuffer();
			void DeleteAllOriginVBData();
		};
	}
}