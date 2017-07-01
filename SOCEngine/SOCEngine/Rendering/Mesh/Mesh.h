#pragma once

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexShader.h"
#include "BufferManager.hpp"
#include "ObjectId.hpp"
#include "Transform.h"
#include "TransformCB.h"
#include "BoundBox.h"
#include <string>

namespace Rendering
{
	namespace Manager
	{
		class MeshManager;
	}

	namespace Geometry
	{
		class Mesh final
		{
		public:
			using ManagerType = Manager::MeshManager;
			Mesh(Core::ObjectId id) : _objectId(id) {}

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
			GET_CONST_ACCESSOR(ObjectId, Core::ObjectId, _objectId);

			void AddMaterialKey(const std::string& key);
			bool HasMaterialKey(const std::string& key) const;
			void DeleteMaterialKey(const std::string& key);

			void CalcWorldSize(Math::Vector3& worldMin, Math::Vector3& worldMax, const Core::Transform& transform);
			void UpdateTransformCB(Device::DirectX& dx, const Core::Transform& transform);

			GET_SET_ACCESSOR(Radius,		float,							_radius);
			GET_SET_ACCESSOR(BoundBox,		const Intersection::BoundBox&,	_boundBox);
			GET_ACCESSOR(PrevWorldMat,		const Math::Matrix&,			_prevWorldMat);

		private:
			uint ComputeBufferFlag(
				const std::vector<Shader::VertexShader::SemanticInfo>& semantics,
				uint maxRecognizeBoneCount = 4) const;

		private:
			Buffer::VertexBuffer						_vertexBuffer;
			Buffer::IndexBuffer							_indexBuffer;
			std::vector<std::string>					_materialKeys;

			uint										_bufferFlag = 0;
			Core::ObjectId								_objectId;
			Buffer::ExplicitConstBuffer<TransformCB>	_transformCB;

			float										_radius = 0.0f;
			Intersection::BoundBox						_boundBox;

			Math::Matrix								_prevWorldMat;
			TransformCB::ChangeState					_tfChangeState;

			bool										_culled = false;

			friend class MeshUtility;
		};
	}
}