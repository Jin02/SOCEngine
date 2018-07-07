#include "MaterialManager.h"

using namespace Device;
using namespace Rendering;
using namespace Rendering::Material;
using namespace Rendering::Manager;

void MaterialManager::Initialize(Device::DirectX& dx)
{
	PhysicallyBasedMaterial materal("@Default");
	materal.Initialize(dx);
	materal.SetMainColor(Color(1.0f, 1.0f, 1.0f, 1.0f));

	_pbmDefaultKey = Add<PhysicallyBasedMaterial>(materal).first;
}

void MaterialManager::Destroy()
{
	auto ClearData = [](auto& datas)
	{		
		datas.dirty.clear();
		datas.idBookmark.DeleteAll();
		datas.idMgr.DeleteAll();

		int count = datas.pool.GetSize();
		for (int i = 0; i < count; ++i)
			datas.pool.Get(i).Destroy();

		datas.pool.DeleteAll();
	};

	ClearData(GetMaterialDatas<PhysicallyBasedMaterial>());
	ClearData(GetMaterialDatas<SkyBoxMaterial>());
}

void MaterialManager::UpdateConstBuffer(DirectX & dx)
{
	auto CheckDirty = [](auto& materialDatas)
	{
		auto& pool	= materialDatas.pool;
		auto& dirty	= materialDatas.dirty;

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