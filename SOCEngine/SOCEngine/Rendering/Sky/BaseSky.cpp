#include "BaseSky.h"
#include "BindIndexInfo.h"
#include "BasicGeometryGenerator.h"

using namespace Rendering::Sky;
using namespace Rendering::Manager;
using namespace Rendering::Buffer;
using namespace Rendering::Shader;
using namespace Rendering::Texture;
using namespace Rendering::Geometry;
using namespace Math;

void BaseSky::Initialize(Device::DirectX& dx, BufferManager& bufferMgr)
{
	auto CreateMeshContent = [this, &dx](const Mesh::CreateFuncArguments& args)
	{
		std::string vbKey = args.fileName + ":" + args.key;

		VertexBuffer::Param param;
		{
			param.key = vbKey;
			param.stride = args.vertices.byteWidth;
			param.vertexCount = args.vertices.count;
		}

		_vertexBuffer.Initialize(dx, param, args.vertices.data, false, args.semanticInfos);
		_indexBuffer.Initialize(dx, args.indices, vbKey, false);
	};

	BasicGeometryGenerator gen;
	gen.CreateSphere(CreateMeshContent, 1.0f, 64, 64, 0);
}