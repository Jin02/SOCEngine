#include "MaterialManager.hpp"

using namespace Device;
using namespace Rendering;
using namespace Rendering::Material;
using namespace Rendering::Manager;

void MaterialManager::UpdateConstBuffer(DirectX & dx)
{
	auto CheckDirty = [](auto& materialDatas)
	{
		auto& pool = materialDatas.pool;
		auto& dirty = materialDatas.dirty;

		uint size = pool.GetSize();
		for (uint i = 0; i<size; ++i)
		{
			auto& material = pool.Get(i);
			if (material.GetDirty())
				dirty.push_back(&material);
		}
	};
	CheckDirty(GetMaterialDatas<PhysicallyBasedMaterial>());

	// Update Buffer
	{
		auto& pbmDirty = GetDirty<PhysicallyBasedMaterial>();
		for (auto& iter : pbmDirty)
			iter->UpdateConstBuffer(dx);
	}

	GetDirty<PhysicallyBasedMaterial>().clear();
}