#include "UnsortedMeshPool.h"

#include "Mesh.h"
#include "ObjectID.hpp"

using namespace Rendering;
using namespace Rendering::Geometry;
using namespace Core;

Mesh& UnsortedMeshPool::Add(Core::ObjectID id, int, Mesh& mesh)
{
	return Parent::Add(id.Literal(), mesh);
}