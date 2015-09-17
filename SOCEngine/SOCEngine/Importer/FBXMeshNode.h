#pragma once

#include <fbxsdk.h>
#include <string>
#include "Common.h"
#include "VectorMap.h"
#include "Vector3.h"
#include "Vector2.h"
#include "Matrix.h"
#include "FBXMaterialNode.h"

namespace Importer
{
	namespace FBX
	{
		class MeshNode
		{
		private:
			std::string						_name;

		private:
			std::vector<uint>				_indices;
			std::vector<Math::Vector3>		_positions;
			std::vector<Math::Vector3>		_normals;
			std::vector<Math::Vector2>		_uvs;

			std::vector<MaterialNode>		_materialNodes;

		private:
			const MeshNode*					_parentMesh;
			std::vector<MeshNode*>			_childs;

		public:
			MeshNode(const MeshNode* parent);
			~MeshNode();

		public:
			GET_SET_ACCESSOR(Name,		const std::string&,					_name);
			GET_ACCESSOR(Indices,		const std::vector<uint>&,			_indices);
			GET_ACCESSOR(Positions,		const std::vector<Math::Vector3>&,	_positions);
			GET_ACCESSOR(Normals,		const std::vector<Math::Vector3>&,	_normals);
			GET_ACCESSOR(UVS,			const std::vector<Math::Vector2>&,	_uvs);
			GET_ACCESSOR(ParentMesh,	const MeshNode*,					_parentMesh);
			GET_ACCESSOR(Childs,		const std::vector<MeshNode*>&,		_childs);
		};
	}
}