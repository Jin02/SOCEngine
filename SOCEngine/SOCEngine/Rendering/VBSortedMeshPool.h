#pragma once

#include "Mesh.h"
#include "VectorIndexer.hpp"

#include "MeshRawPool.h"
#include "Bookmark.hpp"

namespace Rendering
{
	namespace Geometry
	{
		class VBSortedMeshPool final
		{
		public:
			VBSortedMeshPool() = default;

			Mesh&	Add(Core::ObjectID id, Buffer::BaseBuffer::Key vbKey, Mesh& mesh);
			void	Delete(Core::ObjectID::LiteralType literalID);
			void	DeleteAll();
			bool	Has(Core::ObjectID::LiteralType literalID) const;			
			bool	HasVBKey(Buffer::VertexBuffer::Key key) const;

			const Mesh*	Find(Core::ObjectID::LiteralType literalID) const;			
			Mesh*		Find(Core::ObjectID::LiteralType literalID)
			{
				return 
					const_cast<Mesh*>
					(
						static_cast<const MeshRawPool*>(this)->Find(literalID)
					);
			}
			
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
			Core::VectorHashMap<Buffer::VertexBuffer::Key, MeshRawPool>	_pool;
		};
	}
}
