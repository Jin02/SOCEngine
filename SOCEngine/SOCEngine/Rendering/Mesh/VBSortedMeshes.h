#pragma once

#include "Mesh.h"
#include "VectorIndexer.hpp"

#include "MeshRawPool.h"
#include "MeshRawPtrs.h"

#include "Bookmark.hpp"

namespace Rendering
{
	namespace Geometry
	{
		template<typename RawPoolType>
		class VBSortedMeshes final
		{
		public:
			using MappedType = typename RawPoolType::MappedType;
			VBSortedMeshes() = default;

			MappedType& Add(Core::ObjectID id, Buffer::BaseBuffer::Key vbKey, MappedType& mesh)
			{
				auto literalID = id.Literal();
				assert(_bookmark.Has(literalID) == false);

				_bookmark.Add(literalID, vbKey);

				if (HasVBKey(vbKey) == false)
					return _pool.Add(vbKey, { mesh }).Get(0);

				auto rawPool = _pool.Find(vbKey);
				assert(rawPool->Has(literalID) == false);

				return rawPool->Add(mesh);				
			}
			
			void Delete(Core::ObjectID::LiteralType literalID)
			{
				uint vbKey = _bookmark.Find(literalID);
				assert(vbKey != decltype(_bookmark)::Fail());
			
				auto rawPool = _pool.Find(vbKey);
				rawPool->Delete(literalID);

				if (rawPool->Empty())
					_pool.Delete(vbKey);				
				
				_bookmark.Delete(literalID);			
			}
			
			bool Has(Core::ObjectID::LiteralType literalID) const
			{				
				uint vbKey = _bookmark.Find(literalID);
				return (vbKey != decltype(_bookmark)::Fail()) ? _pool.Find(vbKey)->Has(literalID) : false;							
			}
			
			const MappedType* Find(Core::ObjectID::LiteralType literalID) const
			{
				uint vbKey = _bookmark.Find(literalID);
				return (vbKey != decltype(_bookmark)::Fail()) ? _pool.Find(vbKey)->Find(literalID) : nullptr;				
			}

			MappedType* Find(Core::ObjectID::LiteralType literalID)
			{
				return 
					const_cast<MappedType*>
					(
						static_cast<const VBSortedMeshes<RawPoolType>*>(this)->Find(literalID)
					);
			}
			
			
			bool HasVBKey(Buffer::VertexBuffer::Key key) const
			{
				return _pool.Has(key);				
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
			Core::VectorHashMap<Buffer::VertexBuffer::Key, RawPoolType> _pool;
		};

		using OpaqueMeshPool		= VBSortedMeshes<MeshRawPool>;
		using OpaqueMeshPtrs		= VBSortedMeshes<MeshRawPtrs>;

		using AlphaBlendMeshPool	= OpaqueMeshPool;
		using AlphaBlendMeshPtrs	= OpaqueMeshPtrs;
	}
}
