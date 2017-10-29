#include "OpaqueMeshRenderQueue.h"

using namespace Rendering;
using namespace Rendering::Geometry;
using namespace Rendeirng::RenderQueue;

void OpaqueMeshRenderQueue::Add(Mesh& mesh)
{
	auto vbKey = mesh.GetVBKey();
	auto find = _vbPerMeshes.Find(vbKey);

	if(find)	find->Add(&mesh);
	else		_vbPerMeshes.Add(vbKey, { &mesh });
}

void OpaqueMeshRenderQueue::DeleteAllContent()
{
	uint vbKeyCount = _vbPerMeshes.GetSize();
	for (uint i = 0; i < vbKeyCount; ++i)
		_vbPerMeshes.Get(i).DeleteAll();
}
