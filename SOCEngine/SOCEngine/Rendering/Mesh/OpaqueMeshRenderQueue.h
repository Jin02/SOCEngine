#pragma once

#include "VectorIndexer.hpp"
#include "MeshRawPtrs.h"

namespace Rendering
{
	namespace RenderQueue
	{
		class OpaqueMeshRenderQueue final
		{
		public:
			using MeshRenderQType = std::remove_pointer_t<Rendering::Geometry::MeshRawPtrs::PtrType>;

			void Add(MeshRenderQType& mesh);			
			void DeleteAllContent();

			template <class Iterator>		// Iterator Form = "[](const Mesh*) -> void { }"
			void Iterate(Iterator iterator)
			{
				uint vbKeyCount = _vbPerMeshes.GetSize();
				for (uint i = 0; i < vbKeyCount; ++i)
				{
					auto& rawPtrs = _vbPerMeshes.Get(i);

					uint meshCount = rawPtrs.GetSize();
					for (uint meshIdx = 0; meshIdx < meshCount; ++meshIdx)
						iterator( rawPtrs.Get(meshIdx) );
				}
			}			

			template <class Iterator>		// Iterator Form = "[](const Mesh*) -> void { }"
			void Iterate(Iterator iterator) const
			{
				const_cast<OpaqueMeshRenderQueue*>(this)->Iterate(iterator);
			}


			GET_ACCESSOR_REF(Queue,			_vbPerMeshes);
			GET_CONST_ACCESSOR_REF(Queue,	_vbPerMeshes);
			
		private:
			Core::VectorHashMap<Buffer::VertexBuffer::Key,
								Rendering::Geometry::MeshRawPtrs>	_vbPerMeshes;
		};
	}
}
