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
	GetBuffer<SpotLight>().DeleteAll();
	GetPool<SpotLight>().DeleteAll();
	GetDirtyParamLights<SpotLight>().clear();
	GetDirtyTransformLights<SpotLight>().clear();

	GetBuffer<PointLight>().DeleteAll();
	GetPool<PointLight>().DeleteAll();
	GetDirtyParamLights<PointLight>().clear();
	GetDirtyTransformLights<PointLight>().clear();

	GetBuffer<DirectionalLight>().DeleteAll();
	GetPool<DirectionalLight>().DeleteAll();
	GetDirtyParamLights<DirectionalLight>().clear();
	GetDirtyTransformLights<DirectionalLight>().clear();

	GetLightDatas<SpotLight>().reupdateMinIndex			= 0;
	GetLightDatas<PointLight>().reupdateMinIndex		= 0;
	GetLightDatas<DirectionalLight>().reupdateMinIndex	= 0;
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

void LightManager::UpdateParamBuffer(
	const ShadowManager& shadowMgr,
	const Core::ObjectId::IndexHashMap& shaftIndexer)
{
	GetBuffer<DirectionalLight>().
		UpdateLightCommonBuffer(
			GetDirtyParamLights<DirectionalLight>(),
			RequiredIndexer(shadowMgr.GetIndexer<DirectionalLightShadow>(), shaftIndexer),
			GetPool<DirectionalLight>().GetIndexer()
		);

	GetBuffer<PointLight>().
		UpdateLightCommonBuffer(
			GetDirtyParamLights<PointLight>(),
			RequiredIndexer(shadowMgr.GetIndexer<PointLightShadow>(), shaftIndexer),
			GetPool<PointLight>().GetIndexer()
		);

	GetBuffer<SpotLight>().
		UpdateLightCommonBuffer(
			GetDirtyParamLights<SpotLight>(), 
			RequiredIndexer(shadowMgr.GetIndexer<SpotLightShadow>(), shaftIndexer),
			GetPool<SpotLight>().GetIndexer()
		);
}

void LightManager::UpdateSRBuffer(Device::DirectX& dx)
{
	GetBuffer<DirectionalLight>().UpdateSRBuffer(dx);
	GetBuffer<PointLight>().UpdateSRBuffer(dx);
	GetBuffer<SpotLight>().UpdateSRBuffer(dx);
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

			if (i < lightDatas.reupdateMinIndex)
			{
				if (light.GetBase().GetDirty())
					dirtyParamLights.push_back(&light);

				uint objId = light.GetObjectId().Literal();
				auto transform = transformPool.Find(objId);
				if (transform->GetDirty())
					dirtyTFLights.push_back(&light);
			}
			else
			{
				dirtyParamLights.push_back(&light);
				dirtyTFLights.push_back(&light);
			}
		}

		lightDatas.reupdateMinIndex = size;
	};

	UpdateDirtyLight(GetLightDatas<DirectionalLight>());
	UpdateDirtyLight(GetLightDatas<PointLight>());
	UpdateDirtyLight(GetLightDatas<SpotLight>());
}

void LightManager::ClearDirtyLights()
{
	auto UpdateDirtyLight = [](auto& dirtyLights)
	{
		for (auto light : dirtyLights)
			light->GetBase().SetDirty(false);
	};

	UpdateDirtyLight(GetDirtyParamLights<DirectionalLight>());
	UpdateDirtyLight(GetDirtyParamLights<PointLight>());
	UpdateDirtyLight(GetDirtyParamLights<SpotLight>());

	UpdateDirtyLight(GetDirtyTransformLights<DirectionalLight>());
	UpdateDirtyLight(GetDirtyTransformLights<PointLight>());
	UpdateDirtyLight(GetDirtyTransformLights<SpotLight>());
}

void LightManager::BindResources(Device::DirectX& dx, bool bindVS, bool bindGS, bool bindPS)
{
	auto Bind = [&dx, bindVS, bindGS, bindPS](
		TextureBindIndex bind, ShaderResourceBuffer& srb)
	{
		if (bindVS)	VertexShader::BindShaderResourceView(dx, bind, srb.GetShaderResourceView());
		if (bindGS)	GeometryShader::BindShaderResourceView(dx, bind, srb.GetShaderResourceView());
		if (bindPS)	PixelShader::BindShaderResourceView(dx, bind, srb.GetShaderResourceView());
	};

	auto& plBuffer = GetBuffer<PointLight>();
	{
		Bind(TextureBindIndex::PointLightRadiusWithCenter,		plBuffer.GetTransformSRBuffer());
		Bind(TextureBindIndex::PointLightColor,					plBuffer.GetColorSRBuffer());
		Bind(TextureBindIndex::PointLightOptionalParamIndex,	plBuffer.GetOptionalParamIndexSRBuffer());
	}

	auto& dlBuffer = GetBuffer<DirectionalLight>();
	{
		Bind(TextureBindIndex::DirectionalLightDirXY,				dlBuffer.GetTransformSRBuffer());
		Bind(TextureBindIndex::DirectionalLightColor,				dlBuffer.GetColorSRBuffer());
		Bind(TextureBindIndex::DirectionalLightOptionalParamIndex,	dlBuffer.GetOptionalParamIndexSRBuffer());
	}

	auto& slBuffer = GetBuffer<SpotLight>();
	{
		Bind(TextureBindIndex::SpotLightRadiusWithCenter,			slBuffer.GetTransformSRBuffer());
		Bind(TextureBindIndex::SpotLightColor,						slBuffer.GetColorSRBuffer());
		Bind(TextureBindIndex::SpotLightOptionalParamIndex,			slBuffer.GetOptionalParamIndexSRBuffer());
		Bind(TextureBindIndex::SpotLightParam,						slBuffer.GetParamSRBuffer());
	}
}

void LightManager::UnbindResources(Device::DirectX& dx, bool bindVS, bool bindGS, bool bindPS) const
{
	auto Unbind = [&dx, bindVS, bindGS, bindPS](TextureBindIndex bind)
	{
		if(bindVS) VertexShader::UnBindShaderResourceView(dx, bind);
		if(bindPS) PixelShader::UnBindShaderResourceView(dx, bind);
		if(bindGS) GeometryShader::UnBindShaderResourceView(dx, bind);
	};

	Unbind(TextureBindIndex::PointLightRadiusWithCenter);
	Unbind(TextureBindIndex::PointLightColor);
	Unbind(TextureBindIndex::PointLightOptionalParamIndex);

	Unbind(TextureBindIndex::DirectionalLightDirXY);
	Unbind(TextureBindIndex::DirectionalLightColor);
	Unbind(TextureBindIndex::DirectionalLightOptionalParamIndex);

	Unbind(TextureBindIndex::SpotLightRadiusWithCenter);
	Unbind(TextureBindIndex::SpotLightColor);
	Unbind(TextureBindIndex::SpotLightOptionalParamIndex);
	Unbind(TextureBindIndex::SpotLightParam);
}
