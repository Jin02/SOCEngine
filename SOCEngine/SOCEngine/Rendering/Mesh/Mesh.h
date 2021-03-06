#pragma once

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexShader.h"
#include "BufferManager.hpp"
#include "ObjectID.hpp"
#include "Transform.h"
#include "TransformCB.h"
#include "BoundBox.h"
#include "MaterialID.hpp"
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
				const std::string			ibPartID;
				uint						vbUserHashKey;

				const std::vector<Shader::VertexShader::SemanticInfo>& semanticInfos;

				CreateFuncArguments(const std::string& _fileName,
					uint _vbUserHashKey, const std::string& _ibPartID,
					const std::vector<uint>& _indices,
					const std::vector<Shader::VertexShader::SemanticInfo>& _semanticInfos)
					:fileName(_fileName), vbUserHashKey(_vbUserHashKey), ibPartID(_ibPartID), indices(_indices), semanticInfos(_semanticInfos)
				{}
			};
			void Initialize(Device::DirectX& dx, Manager::BufferManager& bufferMgr, const CreateFuncArguments& args);
			void Initialize(Device::DirectX& dx, const Buffer::VertexBuffer::Semantics& semantics, Buffer::BaseBuffer::Key vbKey, Buffer::BaseBuffer::Key ibKey);
			void Destroy();

			void CalcWorldSize(Math::Vector3& worldMin, Math::Vector3& worldMax, const Core::Transform& transform) const;
			void UpdateTransformCB(Device::DirectX& dx, const Core::Transform& transform);

		public:
			GET_CONST_ACCESSOR(ObjectID,		Core::ObjectID,					_objectID);
			GET_CONST_ACCESSOR(PrevWorldMat,	const Math::Matrix&,			_prevWorldMat);

			GET_CONST_ACCESSOR(VBKey,			Buffer::BaseBuffer::Key,		_vbKey);
			GET_CONST_ACCESSOR(IBKey,			Buffer::BaseBuffer::Key,		_ibKey);
			GET_CONST_ACCESSOR(BufferFlag,		uint,							_bufferFlag);

			GET_CONST_ACCESSOR_REF(TransformCB,									_transformCB);

			GET_CONST_ACCESSOR(Radius,			float,							_radius);
			SET_ACCESSOR(Radius,				float,							_radius);

			GET_CONST_ACCESSOR(BoundBox,		const Intersection::BoundBox&,	_boundBox);
			SET_ACCESSOR(BoundBox,				const Intersection::BoundBox&,	_boundBox);

			GET_CONST_ACCESSOR(PBRMaterialID,	MaterialID,						_pbrMaterialID);
			SET_ACCESSOR(PBRMaterialID,			MaterialID,						_pbrMaterialID);

			GET_CONST_ACCESSOR(TFChangeState,	TransformCB::ChangeState,		_tfChangeState);

		private:
			uint ComputeBufferFlag(
				const std::vector<Shader::VertexShader::SemanticInfo>& semantics,
				uint maxRecognizeBoneCount = 4) const;

		private:
			Buffer::BaseBuffer::Key						_vbKey;
			Buffer::BaseBuffer::Key						_ibKey;
			MaterialID									_pbrMaterialID;

			uint										_bufferFlag		= 0;
			Core::ObjectID								_objectID;
			Buffer::ExplicitConstBuffer<TransformCB>	_transformCB;

			float										_radius			= 0.0f;
			Intersection::BoundBox						_boundBox;

			Math::Matrix								_prevWorldMat;
			TransformCB::ChangeState					_tfChangeState	= TransformCB::ChangeState::HasChanged;
		};
	}
}
