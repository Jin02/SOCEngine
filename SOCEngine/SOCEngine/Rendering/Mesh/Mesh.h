#pragma once

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexShader.h"
#include "BufferManager.hpp"
#include "ObjectId.hpp"
#include "TransformCB.h"
#include <string>

namespace Rendering
{
	namespace Geometry
	{
		class Mesh final
		{
		public:
			Mesh(Core::ObjectId id) : _id(id) {}

			struct CreateFuncArguments
			{
				struct Vertices
				{
					const void*		data;
					uint			count;
					uint			byteWidth;
				};
				Vertices					vertices;
				const std::vector<uint>&	indices;
				bool						useDynamicVB	= false;
				bool						useDynamicIB	= false;

				const std::string			fileName;
				const std::string			key;

				const std::vector<Shader::VertexShader::SemanticInfo>& semanticInfos;

				CreateFuncArguments(const std::string& _fileName, const std::string& _key,
					const std::vector<uint>& _indices,
					const std::vector<Shader::VertexShader::SemanticInfo>& _semanticInfos)
					:fileName(_fileName), key(_key), indices(_indices), semanticInfos(_semanticInfos)
				{}
			};
			void Initialize(Device::DirectX& dx, Manager::BufferManager& bufferMgr, const CreateFuncArguments& args);
			void Initialize(const Buffer::VertexBuffer& vertexBuffer, const Buffer::IndexBuffer& indexBuffer);

		public:
			GET_ACCESSOR(MaterialKeys, const std::vector<std::string>&, _materialKeys);
			GET_CONST_ACCESSOR(ObjectId, const Core::ObjectId&, _id);

			void AddMaterialKey(const std::string& key);
			bool HasMaterialKey(const std::string& key) const;
			void DeleteMaterialKey(const std::string& key);

		private:
			uint ComputeBufferFlag(
				const std::vector<Shader::VertexShader::SemanticInfo>& semantics,
				uint maxRecognizeBoneCount = 4) const;

		private:
			Buffer::VertexBuffer						_vertexBuffer;
			Buffer::IndexBuffer							_indexBuffer;
			std::vector<std::string>					_materialKeys;

			uint										_bufferFlag = 0;
			Core::ObjectId								_id;
			Buffer::ExplicitConstBuffer<TransformCB>	_tfCB;
		};
	}
}