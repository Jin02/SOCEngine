#pragma once

#include "Mesh.h"
#include "VectorIndexer.hpp"
#include "MeshRawPool.h"
#include "Bookmark.hpp"

namespace Rendering
{
	namespace Geometry
	{
		using MeshPool = Core::VectorHashMap<Core::ObjectID::LiteralType, Mesh>;

		class VBSortedMeshPool final
		{
		public:
			VBSortedMeshPool() = default;

			Mesh& Add(Mesh& mesh);
			void Delete(Mesh& mesh)		{ Delete(mesh.GetObjectID().Literal());		}
			bool Has(Mesh& mesh) const	{ return Has(mesh.GetObjectID().Literal());	}

			void Delete(Core::ObjectID::LiteralType id);
			bool Has(Core::ObjectID::LiteralType id) const;
			Mesh* Find(Core::ObjectID::LiteralType id);

			bool HasVBKey(Buffer::VertexBuffer::Key key) const;

			template <class Iterator>
			void Iterate(Iterator iterator)
			{
				uint vbKeyCount = _pool.GetSize();
				for (uint i = 0; i < vbKeyCount; ++i)
				{
					auto& rawPool = _pool.Get(i);

					uint meshCount = rawPool.GetSize();
					for (uint meshIdx = 0; meshIdx < meshCount; ++meshIdx)
						iterator( rawPool.Get(meshIdx) );
				}
			}

		private:
			Core::BookHashMapmark<Core::ObjectID::LiteralType>			_bookmark;
			Core::VectorHashMap<Buffer::VertexBuffer::Key, MeshRawPool> _pool;
		};
	}
}