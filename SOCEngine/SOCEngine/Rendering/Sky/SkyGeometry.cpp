#include "SkyGeometry.h"
#include "BindIndexInfo.h"
#include "BasicGeometryGenerator.h"
#include "Utility.hpp"

using namespace Rendering::Sky;
using namespace Rendering::Manager;
using namespace Rendering::Buffer;
using namespace Rendering::Shader;
using namespace Rendering::Texture;
using namespace Rendering::Geometry;
using namespace Math;
using namespace Utility;
using namespace Device;

void SkyGeometry::Initialize(DirectX& dx, BufferManager& bufferMgr)
{
	auto CreateMeshContent = [this, &dx, &bufferMgr](const Mesh::CreateFuncArguments& args)
	{
		uint vbKey = String::MakeKey({"@SkyGeometryVB"});
		uint ibKey = String::MakeKey({"@SkyGeometryIB"});

		auto& vbPool	= bufferMgr.GetPool<VertexBuffer>();
		auto vb			= vbPool.Find(vbKey);

		if (vb == nullptr)
		{
			VertexBuffer::Desc desc;
			{
				desc.stride			= args.vertices.byteWidth;
				desc.vertexCount	= args.vertices.count;
			}

			_vertexBuffer.Initialize(dx, desc, args.vertices.data, false, args.semanticInfos);		
			vbPool.Add(vbKey, _vertexBuffer);
		}
		else
			_vertexBuffer = *vb; 

		auto& ibPool	= bufferMgr.GetPool<IndexBuffer>();
		auto ib			= ibPool.Find(ibKey);

		if (ib == nullptr)
		{
			_indexBuffer.Initialize(dx, args.indices, -1, false);
			ibPool.Add(ibKey, _indexBuffer);
		}
		else
			_indexBuffer = *ib;
	};

	BasicGeometryGenerator::CreateSphere(CreateMeshContent, 1.0f, 64, 64, 0);
}

void SkyGeometry::Destroy()
{
	_vertexBuffer.Destroy();
	_indexBuffer.Destroy();
}
