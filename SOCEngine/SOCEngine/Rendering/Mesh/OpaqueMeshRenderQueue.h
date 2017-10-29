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
			void Add(Mesh& mesh);			
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

			
		private:
			Core::VectorHashMap<Buffer::VertexBuffer::Key,
								Rendering::Geometry::MeshRawPtrs>	_vbPerMeshes;
		}
	}
}
