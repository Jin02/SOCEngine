#include "BufferManager.h"
#include <assert.h>

using namespace Rendering;
using namespace Rendering::Buffer;
using namespace Manager;

void BufferManager::Destroy()
{
	_vertexBuffers.Destroy();
	_indexBuffers.Destroy();
	_originVertexBufferDatas.Destroy();
}