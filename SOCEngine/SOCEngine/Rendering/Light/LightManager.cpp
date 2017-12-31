#include "LightManager.h"

#include "VertexShader.h"
#include "PixelShader.h"
#include "GeometryShader.h"

#include "ShadowManager.h"

#include <type_traits>

using namespace Core;
using namespace Rendering;
using namespace Rendering::Shader;
using namespace Rendering::Buffer;
using namespace Rendering::View;
using namespace Rendering::Manager;
using namespace Rendering::Light;
using namespace Rendering::Light::Buffer;
using namespace Rendering::Shadow;

void LightManager::Initialize(Device::DirectX& dx)
{
	GetBuffer<DirectionalLight>().Initialize(dx);
	GetBuffer<PointLight>().Initialize(dx);
	GetBuffer<SpotLight>().Initialize(dx);
}

void LightManager::DeleteAll()
{
	auto DeleteAll = [](auto& lightData)
	{
		lightData.lightBuffer.DeleteAll();
		lightData.pool.DeleteAll();
		lightData.dirtyParamLights.clear();
		lightData.dirtyTransformLights.clear();

		lightData.mustUpdateToGPU = true;
	};

	DeleteAll(GetLightDatas<SpotLight>());
	DeleteAll(GetLightDatas<PointLight>());
	DeleteAll(GetLightDatas<DirectionalLight>());

	_changedLightCounts = true;
}

uint LightManager::GetPackedLightCount() const
{
	uint spotLightCount			= GetLightCount<SpotLight>()		& 0x7FF;
	uint pointLightCount		= GetLightCount<PointLight>()		& 0x7FF;
	uint directionalLightCount	= GetLightCount<DirectionalLight>()	& 0x3FF;

	return (pointLightCount << 21) | (spotLightCount << 10) | directionalLightCount;
}

void LightManager::UpdateTransformBuffer(const Core::TransformPool& transformPool)
{
	GetBuffer<SpotLight>().UpdateTransformBuffer(GetDirtyTransformLights<SpotLight>(), transformPool, GetPool<SpotLight>().GetIndexer());
	GetBuffer<PointLight>().UpdateTransformBuffer(GetDirtyTransformLights<PointLight>(), transformPool, GetPool<PointLight>().GetIndexer());
	GetBuffer<DirectionalLight>().UpdateTransformBuffer(GetDirtyTransformLights<DirectionalLight>(), transformPool, GetPool<DirectionalLight>().GetIndexer());
}

void LightManager::UpdateParamBuffer(const ShadowManager& shadowMgr)
{
	GetBuffer<DirectionalLight>().
		UpdateLightCommonBuffer(
			GetDirtyParamLights<DirectionalLight>(),
			shadowMgr.GetIndexer<DirectionalLightShadow>(),
			GetPool<DirectionalLight>().GetIndexer()
		);

	GetBuffer<PointLight>().
		UpdateLightCommonBuffer(
			GetDirtyParamLights<PointLight>(),
			shadowMgr.GetIndexer<PointLightShadow>(),
			GetPool<PointLight>().GetIndexer()
		);

	GetBuffer<SpotLight>().
		UpdateLightCommonBuffer(
			GetDirtyParamLights<SpotLight>(), 
			shadowMgr.GetIndexer<SpotLightShadow>(),
			GetPool<SpotLight>().GetIndexer()
		);
}

void LightManager::UpdateSRBuffer(Device::DirectX& dx)
{
	auto UpdateSRBuffer = [&dx](auto& ligtDatas)
	{
		ligtDatas.lightBuffer.UpdateSRBuffer(dx, ligtDatas.mustUpdateToGPU);
		ligtDatas.mustUpdateToGPU = false;
	};

	UpdateSRBuffer(GetLightDatas<DirectionalLight>());
	UpdateSRBuffer(GetLightDatas<PointLight>());
	UpdateSRBuffer(GetLightDatas<SpotLight>());
}

void LightManager::CheckDirtyLights(const Core::TransformPool& transformPool)
{
	auto UpdateDirtyLight = [&transformPool](auto& lightDatas)
	{
		auto& pool = lightDatas.pool;
		auto& dirtyParamLights = lightDatas.dirtyParamLights;
		auto& dirtyTFLights = lightDatas.dirtyTransformLights;

		uint size = pool.GetSize();
		for (uint i = 0; i < size; ++i)
		{
			auto& light = pool.Get(i);

			if (light.GetBase()->GetDirty())
				dirtyParamLights.push_back(&light);

			uint objID		= light.GetObjectID().Literal();
			auto transform	= transformPool.Find(objID);
			if (transform->GetDirty())
				dirtyTFLights.push_back(&light);
		}
	};

	UpdateDirtyLight(GetLightDatas<DirectionalLight>());
	UpdateDirtyLight(GetLightDatas<PointLight>());
	UpdateDirtyLight(GetLightDatas<SpotLight>());
}

void LightManager::ClearDirty()
{
	auto UpdateDirtyLight = [](auto& dirtyLights)
	{
		for (auto light : dirtyLights)
			light->GetBase()->SetDirty(false);

		dirtyLights.clear();
	};

	UpdateDirtyLight(GetDirtyParamLights<DirectionalLight>());
	UpdateDirtyLight(GetDirtyParamLights<PointLight>());
	UpdateDirtyLight(GetDirtyParamLights<SpotLight>());

	UpdateDirtyLight(GetDirtyTransformLights<DirectionalLight>());
	UpdateDirtyLight(GetDirtyTransformLights<PointLight>());
	UpdateDirtyLight(GetDirtyTransformLights<SpotLight>());

	_changedLightCounts = false;
}