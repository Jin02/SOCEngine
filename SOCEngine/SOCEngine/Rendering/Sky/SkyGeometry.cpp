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

void SkyGeometry::Initialize(Device::DirectX& dx)
{
	auto CreateMeshContent = [this, &dx](const Mesh::CreateFuncArguments& args)
	{
		VertexBuffer::Desc desc;
		{
			desc.stride			= args.vertices.byteWidth;
			desc.vertexCount	= args.vertices.count;
		}

		_vertexBuffer.Initialize(dx, desc, args.vertices.data, false, args.semanticInfos);		
		_indexBuffer.Initialize(dx, args.indices, -1, false);
	};

	BasicGeometryGenerator::CreateSphere(CreateMeshContent, 1.0f, 64, 64, 0);
}

void SkyGeometry::Destroy()
{
	_vertexBuffer.Destroy();
	_indexBuffer.Destroy();
}
