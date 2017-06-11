#include "LightManager.h"

#include "VertexShader.h"
#include "PixelShader.h"
#include "GeometryShader.h"



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
	GetBuffer<DirectionalLight>().Initialize(dx);
	GetBuffer<PointLight>().Initialize(dx);
	GetBuffer<SpotLight>().Initialize(dx);
}

uint LightManager::GetPackedLightCount() const
{
	uint directionalLightCount = GetBuffer<DirectionalLight>().GetSize() & 0x3FF;
	uint pointLightCount = GetBuffer<PointLight>().GetSize() & 0x7FF;
	uint spotLightCount = GetBuffer<SpotLight>().GetSize() & 0x7FF;

	return (pointLightCount << 21) | (spotLightCount << 10) | directionalLightCount;
}

void LightManager::UpdateTransformBuffer(const std::vector<Core::Transform*>& dirtyTransforms)
{
	GetBuffer<DirectionalLight>().GetLightingBuffer().
		UpdateTransformBuffer(dirtyTransforms, GetPool<DirectionalLight>());
	GetBuffer<PointLight>().GetLightingBuffer().
		UpdateTransformBuffer(dirtyTransforms, GetPool<PointLight>());
	GetBuffer<SpotLight>().GetLightingBuffer().
		UpdateTransformBuffer(dirtyTransforms, GetPool<SpotLight>());
}

void LightManager::UpdateParamBuffer(
	const LightingBuffer::RequiredIndexBook& indexBooks,
	const Core::TransformPool& transformPool)
{
	GetBuffer<DirectionalLight>().GetLightingBuffer().
		UpdateLightCommonBuffer(GetDirtyLights<DirectionalLight>(), indexBooks);

	GetBuffer<PointLight>().GetLightingBuffer().
		UpdateLightCommonBuffer(GetDirtyLights<PointLight>(), indexBooks);

	auto& spotLightingBuffer = GetBuffer<SpotLight>().GetLightingBuffer();
	const auto& dirtySpotLights = GetDirtyLights<SpotLight>();
	spotLightingBuffer.UpdateLightCommonBuffer(dirtySpotLights, indexBooks);
	spotLightingBuffer.UpdateParamBuffer(dirtySpotLights, transformPool);
}

void LightManager::UpdateSRBuffer(Device::DirectX& dx)
{
	GetBuffer<DirectionalLight>().GetLightingBuffer().UpdateSRBuffer(dx);
	GetBuffer<PointLight>().GetLightingBuffer().UpdateSRBuffer(dx);
	GetBuffer<SpotLight>().GetLightingBuffer().UpdateSRBuffer(dx);
}

void LightManager::CheckDirtyLights()
{
	auto UpdateDirtyLight = [](auto& pool, auto& dirtyLights)
	{
		uint size = pool.GetSize();
		for (uint i = 0; i < size; ++i)
		{
			auto& light = pool.Get(i);
			if (light.GetBase().GetDirty())
				dirtyLights.push_back(&light);
		}
	};

	UpdateDirtyLight(GetPool<DirectionalLight>(),	GetDirtyLights<DirectionalLight>());
	UpdateDirtyLight(GetPool<PointLight>(),			GetDirtyLights<PointLight>());
	UpdateDirtyLight(GetPool<SpotLight>(),			GetDirtyLights<SpotLight>());
}

void LightManager::ClearDirtyLights()
{
	auto UpdateDirtyLight = [](auto& dirtyLights)
	{
		for (auto light : dirtyLights)
			light->GetBase().SetDirty(false);
	};

	UpdateDirtyLight(GetDirtyLights<DirectionalLight>());
	UpdateDirtyLight(GetDirtyLights<PointLight>());
	UpdateDirtyLight(GetDirtyLights<SpotLight>());
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
