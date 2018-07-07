#include "DirectionalLightShadowMapCBPool.h"

using namespace Device;
using namespace Rendering::Shadow::Buffer;

void DirectionalLightShadowMapCBPool::PushConstBufferToQueue()
{
	_preparedConstBuffers.push_back({ConstBufferType(), ConstBufferType()});
}

void DirectionalLightShadowMapCBPool::InitializePreparedCB(DirectX & dx)
{
	if (_preparedConstBuffers.empty())
		return;

	for (auto& iter : _preparedConstBuffers)
	{
		iter.viewProjMatCB.Initialize(dx);
		iter.viewMatCB.Initialize(dx);
	}

	_constBuffers.insert(_constBuffers.end(), _preparedConstBuffers.begin(), _preparedConstBuffers.end());
	_preparedConstBuffers.clear();
}

void DirectionalLightShadowMapCBPool::Delete(uint shadowIndex)
{
	auto iter = _constBuffers.begin() + shadowIndex;
	_constBuffers.erase(iter);
}

void DirectionalLightShadowMapCBPool::DeleteAll()
{
	for (auto& iter : _constBuffers)
	{
		iter.viewMatCB.Destroy();
		iter.viewProjMatCB.Destroy();
	}
	_constBuffers.clear();
	_preparedConstBuffers.clear();
}
