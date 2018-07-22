#pragma once

#include "VectorIndexer.hpp"
#include "ObjectID.hpp"

namespace Rendering
{
	namespace Geometry
	{
		class Mesh;

		class UnsortedMeshPool final 
			: public Core::VectorHashMap<Core::ObjectID::LiteralType, Mesh>
		{
		public:
			using Parent = Core::VectorHashMap<Core::ObjectID::LiteralType, Mesh>;
			using Parent::Parent;

		public:
			// int�� VBSortedPool�� interface ���ߴ� �뵵�� �Ұ���.
			Mesh& Add(Core::ObjectID id, int, Mesh& mesh);
		};
	}
}
