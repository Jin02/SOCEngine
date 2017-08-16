#pragma once

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexShader.h"
#include "BufferManager.hpp"
#include "ObjectID.hpp"
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
		enum class Trait
		{
			Opaque,
			AlphaBlend,
			Transparency
		};
		struct OpaqueTrait		
		{ constexpr static Trait Trait = Trait::Opaque;			};
		struct AlphaBlendTrait	
		{ constexpr static Trait Trait = Trait::AlphaBlend;		};
		struct TransparencyTrait
		{ constexpr static Trait Trait = Trait::Transparency;	};

		class Mesh final
		{
		public:
			using ManagerType = Manager::MeshManager;
			Mesh(Core::ObjectID id) : _objectID(id) {}

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

			void CalcWorldSize(Math::Vector3& worldMin, Math::Vector3& worldMax, const Core::Transform& transform) const;
			void UpdateTransformCB(Device::DirectX& dx, const Core::Transform& transform);

			template <typename ToTrait>
			void ChangeTrait()
			{
				if (_trait == ToTrait::Trait)
					return;

				_trait = ToTrait::Trait;
				_changedTrait = true;
			}
			void ClearDirty() { _changedTrait = false; }

		public:
			void AddMaterialKey(const std::string& key);
			bool HasMaterialKey(const std::string& key) const;
			void DeleteMaterialKey(const std::string& key);

		public:
			GET_ACCESSOR(MaterialKeys, const std::vector<std::string>&, _materialKeys);
			GET_CONST_ACCESSOR(ObjectID, Core::ObjectID, _objectID);

			GET_SET_ACCESSOR(Radius,		float,							_radius);
			GET_SET_ACCESSOR(BoundBox,		const Intersection::BoundBox&,	_boundBox);
			GET_ACCESSOR(PrevWorldMat,		const Math::Matrix&,			_prevWorldMat);

			GET_CONST_ACCESSOR(ChangedTrait, bool, _changedTrait);
			GET_CONST_ACCESSOR(Trait, Trait, _trait);

			GET_ACCESSOR(VertexBuffer, auto&, _vertexBuffer);
			GET_ACCESSOR(IndexBuffer, auto&, _indexBuffer);
			GET_ACCESSOR(TransformCB, auto&, _transformCB);

			GET_CONST_ACCESSOR(BufferFlag, uint, _bufferFlag);

		private:
			uint ComputeBufferFlag(
				const std::vector<Shader::VertexShader::SemanticInfo>& semantics,
				uint maxRecognizeBoneCount = 4) const;

		private:
			Buffer::VertexBuffer						_vertexBuffer;
			Buffer::IndexBuffer							_indexBuffer;
			std::vector<std::string>					_materialKeys;

			uint										_bufferFlag = 0;
			Core::ObjectID								_objectID;
			Buffer::ExplicitConstBuffer<TransformCB>	_transformCB;

			float										_radius = 0.0f;
			Intersection::BoundBox						_boundBox;

			Math::Matrix								_prevWorldMat;
			TransformCB::ChangeState					_tfChangeState;

			bool										_culled = false;

			Trait										_trait = Trait::Opaque;
			bool										_changedTrait = true;

			friend class MeshUtility;
		};
	}
}