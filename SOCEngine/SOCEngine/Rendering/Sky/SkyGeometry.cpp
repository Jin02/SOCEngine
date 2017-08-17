#include "SkyGeometry.h"
#include "BindIndexInfo.h"
#include "BasicGeometryGenerator.h"

using namespace Rendering::Sky;
using namespace Rendering::Manager;
using namespace Rendering::Buffer;
using namespace Rendering::Shader;
using namespace Rendering::Texture;
using namespace Rendering::Geometry;
using namespace Math;

void SkyGeometry::Initialize(Device::DirectX& dx, BufferManager& bufferMgr)
{
	auto CreateMeshContent = [this, &dx](const Mesh::CreateFuncArguments& args)
	{
		uint vbKey = VBPool::MakeKey(args.fileName, args.vbChunkKey);

		VertexBuffer::Desc desc;
		{
			desc.key = vbKey;
			desc.stride = args.vertices.byteWidth;
			desc.vertexCount = args.vertices.count;
		}

		_vertexBuffer.Initialize(dx, desc, args.vertices.data, false, args.semanticInfos);
		_indexBuffer.Initialize(dx, args.indices, args.vbChunkKey, false);
	};

	BasicGeometryGenerator gen;
	gen.CreateSphere(CreateMeshContent, 1.0f, 64, 64, 0);
}