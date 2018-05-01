#pragma once

#include "Mesh.h"
#include "VectorIndexer.hpp"

#include "MeshRawPool.h"
#include "Bookmark.hpp"

namespace Rendering
{
	namespace Geometry
	{
		class VBSortedMeshPool
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
						static_cast<const VBSortedMeshPool*>(this)->Find(literalID)
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
						iterator(rawPool.Get(meshIdx));
				}
			}

			template <class Iterator>
			void Iterate(Iterator iterator) const
			{
				const_cast<VBSortedMeshPool*>(this)->Iterate(iterator);
			}

			GET_CONST_ACCESSOR(Pool,	_pool);
			GET_ACCESSOR(Pool,			_pool);

		private:
			using VBKey = Buffer::VertexBuffer::Key;
			Core::BookHashMapmark<Core::ObjectID::LiteralType, VBKey>	_bookmark;
			Core::VectorHashMap<VBKey, MeshRawPool>						_pool;
		};
	}
}
