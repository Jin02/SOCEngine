#include "SpotLightShadowMapCBPool.h"

using namespace Device;
using namespace Rendering::Shadow::Buffer;

void SpotLightShadowMapCBPool::PushConstBufferToQueue()
{
	_preparedConstBuffers.push_back(ConstBufferType());
}

void SpotLightShadowMapCBPool::InitializePreparedCB(DirectX & dx)
{
	if (_preparedConstBuffers.empty())
		return;

	for (auto& iter : _preparedConstBuffers)
		iter.Initialize(dx);

	_constBuffers.insert(_constBuffers.end(), _preparedConstBuffers.begin(), _preparedConstBuffers.end());
	_preparedConstBuffers.clear();
}

void SpotLightShadowMapCBPool::Delete(uint shadowIndex)
{
	auto iter = _constBuffers.begin() + shadowIndex;
	_constBuffers.erase(iter);
}

void SpotLightShadowMapCBPool::DeleteAll()
{
	_constBuffers.clear();
	_preparedConstBuffers.clear();
}
