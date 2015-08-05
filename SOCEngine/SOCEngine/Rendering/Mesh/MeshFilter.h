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
		public:
			enum class BufferElement : unsigned int
			{
				Normal		= 1,
				UV			= 2,
				Tangent		= 4,
				Binormal	= 8,
				Custom		= 16
			};
			typedef unsigned int BufferElementFlag;

		private:
			Buffer::VertexBuffer*	_vertexBuffer;
			Buffer::IndexBuffer*	_indexBuffer;

			BufferElementFlag		_bufferFlag;

		private:
			bool			_alloc;
			unsigned int	_vertexCount;
			unsigned int	_indexCount;

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
				MeshFilter::BufferElementFlag	bufferFlag;

				const std::string				fileName;
				const std::string				key;

				CreateFuncArguments(const std::string& _fileName, const std::string& _key)
					:fileName(_fileName), key(_key) {}
				~CreateFuncArguments() {}
			};
			bool CreateBuffer(const CreateFuncArguments& args);

			void IASetBuffer(const Device::DirectX* dx);
			void UpdateVertexBufferData(const Device::DirectX* dx, const void* data, uint size);

		public:
			GET_ACCESSOR(BufferElementFlag, BufferElementFlag, _bufferFlag);
			GET_ACCESSOR(VertexCount, unsigned int, _vertexCount);
			GET_ACCESSOR(IndexCount, unsigned int, _indexCount);
		};
	}
}