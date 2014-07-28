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

			Shader::VertexShader*	_vertexShader;
			Shader::PixelShader*	_pixelShader;

		private:
			bool _alloc;
			unsigned int _numOfVertex;

		public:
			MeshFilter() : _vertexBuffer(nullptr), _vertexShader(nullptr), _indexBuffer(nullptr)
			{
				_alloc = false;
				_numOfVertex = 0;
			}

			~MeshFilter()
			{
				if(_alloc == false)
					return;

				SAFE_DELETE(_vertexBuffer);
				SAFE_DELETE(_vertexShader);
			}

		public:
			//struct BaseMeshInfo
			//{
			//	const Math::Vector3 vertex;
			//	const Math::Vector3 normal;
			//	const Math::Vector3 tangent;
			//	const Math::Vector3 binormal;
			//	const Color color;
			//	std::vector<const Math::Vector2> texCoords;
			//};

			bool CreateBuffer(
				const void* vertexBufferDatas,
				unsigned int vertexBufferCount, 
				unsigned int vertexBufferSize,
				WORD* indices, unsigned int indexCount,
				bool isDynamic)
			{
				if(vertexBufferDatas == nullptr)
					return false;

				_numOfVertex = vertexBufferCount;

				_vertexBuffer = new Buffer::VertexBuffer;
				if( _vertexBuffer->Create(vertexBufferDatas, vertexBufferCount * vertexBufferSize, isDynamic) == false )
					return false;

				_indexBuffer = new Buffer::IndexBuffer;
				if( _indexBuffer->Create(indices, sizeof(WORD) * indexCount) == false )
					return false;

				return true;
			}

			bool CreateShader(const D3D11_INPUT_ELEMENT_DESC* vertexDeclations, unsigned int count)
			{
				_vertexShader = new Shader::VertexShader;
				if(_vertexShader->Create(vertexDeclations, count) == false)
					return false;

				_pixelShader = new Shader::PixelShader;
				if(_pixelShader->Create() == false)
					return false;

				return true;
			}
		};
	}
}