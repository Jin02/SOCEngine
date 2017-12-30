#include "PointLightShadowMapCBPool.h"

using namespace Device;
using namespace Rendering::Shadow;
using namespace Rendering::Shadow::Buffer;

void PointLightShadowMapCBPool::PushConstBufferToQueue()
{
	_preparedConstBuffers.push_back(ConstBufferType());
}

void PointLightShadowMapCBPool::InitializePreparedCB(DirectX& dx)
{
	if (_preparedConstBuffers.empty())
		return;

	for (auto& iter : _preparedConstBuffers)
	{
		for (uint i = 0; i < 6; ++i)
			iter[i].Initialize(dx);
	}

	_constBuffers.insert(_constBuffers.end(), _preparedConstBuffers.begin(), _preparedConstBuffers.end());
	_preparedConstBuffers.clear();
}

void PointLightShadowMapCBPool::Delete(uint index)
{
	auto iter = _constBuffers.begin() + index;
	_constBuffers.erase(iter);
}

void PointLightShadowMapCBPool::DeleteAll()
{
	_constBuffers.clear();
	_preparedConstBuffers.clear();
}

void PointLightShadowMapCBPool::UpdateSubResource(DirectX& dx, const std::vector<PointLightShadow*>& dirtyShadows, const ShadowDatasIndexer& indexer)
{
	for (auto shadow : dirtyShadows)
	{
		Core::ObjectID objID = shadow->GetObjectID();
		uint index = indexer.Find(objID.Literal());

		for (uint i = 0; i < 6; ++i)
			_constBuffers[index][i].UpdateSubResource(dx, shadow->GetTransposedVPMat()[i]);
	}
}
