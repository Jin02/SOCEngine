#include "LightManager.h"

#include "VertexShader.h"
#include "PixelShader.h"
#include "GeometryShader.h"

#include <type_traits>

using namespace Core;
using namespace Rendering;
using namespace Rendering::Shader;
using namespace Rendering::Buffer;
using namespace Rendering::View;
using namespace Rendering::Manager;
using namespace Rendering::Light;
using namespace Rendering::Light::LightingBuffer;

void LightManager::Initialize(Device::DirectX& dx)
{
	GetBuffer<DirectionalLight>().GetLightingBuffer().Initialize(dx);
	GetBuffer<PointLight>().GetLightingBuffer().Initialize(dx);
	GetBuffer<SpotLight>().GetLightingBuffer().Initialize(dx);
}

void LightManager::DeleteAll()
{
	GetBuffer<DirectionalLight>().GetLightingBuffer().DeleteAll();
	GetPool<DirectionalLight>().DeleteAll();
	GetDirtyParamLights<DirectionalLight>().clear();
	GetDirtyTFLights<DirectionalLight>().clear();

	GetBuffer<SpotLight>().GetLightingBuffer().DeleteAll();
	GetPool<SpotLight>().DeleteAll();
	GetDirtyParamLights<SpotLight>().clear();
	GetDirtyTFLights<SpotLight>().clear();

	GetBuffer<PointLight>().GetLightingBuffer().DeleteAll();
	GetPool<PointLight>().DeleteAll();
	GetDirtyParamLights<PointLight>().clear();
	GetDirtyTFLights<PointLight>().clear();

	_idMgr.DeleteAll();
}

uint LightManager::GetPackedLightCount() const
{
	uint directionalLightCount = GetBuffer<DirectionalLight>().GetSize() & 0x3FF;
	uint pointLightCount = GetBuffer<PointLight>().GetSize() & 0x7FF;
	uint spotLightCount = GetBuffer<SpotLight>().GetSize() & 0x7FF;

	return (pointLightCount << 21) | (spotLightCount << 10) | directionalLightCount;
}

void LightManager::UpdateTransformBuffer(const Core::TransformPool& transformPool)
{
	GetBuffer<DirectionalLight>().GetLightingBuffer().
		UpdateTransformBuffer(GetDirtyTFLights<DirectionalLight>(), transformPool);
	GetBuffer<PointLight>().GetLightingBuffer().
		UpdateTransformBuffer(GetDirtyTFLights<PointLight>(), transformPool);
	GetBuffer<SpotLight>().GetLightingBuffer().
		UpdateTransformBuffer(GetDirtyTFLights<SpotLight>(), transformPool);
}

void LightManager::UpdateParamBuffer(
	const LightingBuffer::RequiredIndexBook& indexBooks,
	const Core::TransformPool& transformPool)
{
	GetBuffer<DirectionalLight>().GetLightingBuffer().
		UpdateLightCommonBuffer(GetDirtyParamLights<DirectionalLight>(), indexBooks);

	GetBuffer<PointLight>().GetLightingBuffer().
		UpdateLightCommonBuffer(GetDirtyParamLights<PointLight>(), indexBooks);

	GetBuffer<SpotLight>().GetLightingBuffer().
		UpdateLightCommonBuffer(GetDirtyParamLights<SpotLight>(), indexBooks);
}

void LightManager::UpdateSRBuffer(Device::DirectX& dx)
{
	GetBuffer<DirectionalLight>().GetLightingBuffer().UpdateSRBuffer(dx);
	GetBuffer<PointLight>().GetLightingBuffer().UpdateSRBuffer(dx);
	GetBuffer<SpotLight>().GetLightingBuffer().UpdateSRBuffer(dx);
}

void LightManager::CheckDirtyLights(const Core::TransformPool& transformPool)
{
	auto UpdateDirtyLight = [&transformPool](auto& pool, auto& dirtyParamLights, auto& dirtyTFLights)
	{
		uint size = pool.GetSize();
		for (uint i = 0; i < size; ++i)
		{
			auto& light = pool.Get(i);
			if (light.GetBase().GetDirty())
				dirtyParamLights.push_back(&light);

			uint objId = light.GetObjectId().Literal();
			auto transform = transformPool.Find(objId);
			if (transform->GetDirty())
				dirtyTFLights.push_back(&light);
		}
	};

	UpdateDirtyLight(GetPool<DirectionalLight>(),	GetDirtyParamLights<DirectionalLight>(),	GetDirtyTFLights<DirectionalLight>());
	UpdateDirtyLight(GetPool<PointLight>(),			GetDirtyParamLights<PointLight>(),			GetDirtyTFLights<PointLight>());
	UpdateDirtyLight(GetPool<SpotLight>(),			GetDirtyParamLights<SpotLight>(),			GetDirtyTFLights<SpotLight>());
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

	UpdateDirtyLight(GetDirtyTFLights<DirectionalLight>());
	UpdateDirtyLight(GetDirtyTFLights<PointLight>());
	UpdateDirtyLight(GetDirtyTFLights<SpotLight>());
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

	auto& plBuffer = GetBuffer<PointLight>().GetLightingBuffer();
	{
		Bind(TextureBindIndex::PointLightRadiusWithCenter,		plBuffer.GetTransformSRBuffer());
		Bind(TextureBindIndex::PointLightColor,					plBuffer.GetColorSRBuffer());
		Bind(TextureBindIndex::PointLightOptionalParamIndex,	plBuffer.GetOptionalParamIndexSRBuffer());
	}

	auto& dlBuffer = GetBuffer<DirectionalLight>().GetLightingBuffer();
	{
		Bind(TextureBindIndex::DirectionalLightDirXY,				dlBuffer.GetTransformSRBuffer());
		Bind(TextureBindIndex::DirectionalLightColor,				dlBuffer.GetColorSRBuffer());
		Bind(TextureBindIndex::DirectionalLightOptionalParamIndex,	dlBuffer.GetOptionalParamIndexSRBuffer());
	}

	auto& slBuffer = GetBuffer<SpotLight>().GetLightingBuffer();
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
