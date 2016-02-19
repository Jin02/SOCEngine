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
	namespace Geometry
	{
		typedef unsigned int Count; 

		class MeshFilter
		{
		private:
			Buffer::VertexBuffer*	_vertexBuffer;
			Buffer::IndexBuffer*	_indexBuffer;

		private:
			bool			_alloc;
			uint			_bufferFlag;

		public:
			MeshFilter();
			~MeshFilter();

		private:
			uint ComputeBufferFlag(const std::vector<Shader::VertexShader::SemanticInfo>& semantics) const;

		public:
			struct CreateFuncArguments
			{
				struct Vertices
				{
					const void*		data;
					unsigned int	count;
					unsigned int	byteWidth;
				};
				Vertices						vertices;
				std::vector<uint>*				indices;
				bool							useDynamicVB;
				bool							useDynamicIB;

				const std::string				fileName;
				const std::string				key;

				const std::vector<Rendering::Shader::VertexShader::SemanticInfo>* semanticInfos;

				CreateFuncArguments(const std::string& _fileName, const std::string& _key)
					:fileName(_fileName), key(_key),
					useDynamicVB(false), useDynamicIB(false), semanticInfos(nullptr) {}
				~CreateFuncArguments() {}
			};
			bool Initialize(const CreateFuncArguments& args);
			bool Initialize(Rendering::Buffer::VertexBuffer*& vertexBuffer, Rendering::Buffer::IndexBuffer*& indexBuffer);

		public:
			GET_ACCESSOR(BufferFlag,	uint,					_bufferFlag);
			GET_ACCESSOR(IndexCount,	uint,					_indexBuffer->GetIndexCount());
			GET_ACCESSOR(VertexCount,	uint,					_vertexBuffer->GetVertexCount());
			GET_ACCESSOR(VertexBuffer,	Buffer::VertexBuffer*,	_vertexBuffer);
			GET_ACCESSOR(IndexBuffer,	Buffer::IndexBuffer*,	_indexBuffer);
		};
	}
}