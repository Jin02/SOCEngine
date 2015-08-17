#pragma once

#include "VertexBuffer.h"
#include "IndexBuffer.h"

#include "VertexShader.h"
#include "PixelShader.h"

#include "EngineMath.h"
#include "Color.h"
#include <vector>

namespace Rendering
{
	namespace Mesh
	{
		typedef unsigned int Count; 

		class MeshFilter
		{
		private:
			Buffer::VertexBuffer*	_vertexBuffer;
			Buffer::IndexBuffer*	_indexBuffer;

		private:
			bool			_alloc;
			unsigned int	_vertexCount;
			unsigned int	_indexCount;
			uint			_bufferFlag;

		public:
			MeshFilter();
			~MeshFilter();

		public:
			struct CreateFuncArguments
			{
				template <typename Type>
				struct Buffer
				{
					const Type* data;
					unsigned int count;
					unsigned int byteWidth;
				};
				Buffer<void>					vertex;
				Buffer<unsigned int>			index;
				bool							isDynamic;

				const std::string				fileName;
				const std::string				key;
				uint							bufferFlag;

				CreateFuncArguments(const std::string& _fileName, const std::string& _key)
					:fileName(_fileName), key(_key), bufferFlag(0) {}
				~CreateFuncArguments() {}
			};
			bool CreateBuffer(const CreateFuncArguments& args);

			void IASetBuffer(const Device::DirectX* dx);
			void UpdateVertexBufferData(const Device::DirectX* dx, const void* data, uint size);

		public:
			GET_ACCESSOR(VertexCount, unsigned int, _vertexCount);
			GET_ACCESSOR(IndexCount, unsigned int, _indexCount);
			GET_ACCESSOR(BufferFlag, uint, _bufferFlag);
		};
	}
}