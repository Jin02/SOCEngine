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
			// int는 VBSortedPool과 interface 맞추는 용도에 불과함.
			Mesh& Add(Core::ObjectID id, int, Mesh& mesh);
		};
	}
}
