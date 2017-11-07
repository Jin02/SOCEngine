#include "ShadowMapCBPool.h"

using namespace Device;
using namespace Rendering::Shadow::Buffer;

void ShadowMapCBPool::PushConstBufferToQueue()
{
	_preparedConstBuffers.push_back(ConstBufferType());
}

void ShadowMapCBPool::InitializePreparedCB(DirectX & dx)
{
	if (_preparedConstBuffers.empty())
		return;

	for (auto& iter : _preparedConstBuffers)
		iter.Initialize(dx);

	_constBuffers.insert(_constBuffers.end(), _preparedConstBuffers.begin(), _preparedConstBuffers.end());
	_preparedConstBuffers.clear();
}

void ShadowMapCBPool::Delete(uint shadowIndex)
{
	auto iter = _constBuffers.begin() + shadowIndex;
	_constBuffers.erase(iter);
}

void ShadowMapCBPool::DeleteAll()
{
	_constBuffers.clear();
	_preparedConstBuffers.clear();
}
