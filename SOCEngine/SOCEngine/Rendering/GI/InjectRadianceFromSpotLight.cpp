#include "InjectRadianceFromSpotLight.h"
#include "BindIndexInfo.h"
#include "EngineShaderFactory.hpp"

using namespace Device;
using namespace Math;
using namespace Core;
using namespace Rendering;
using namespace Rendering::Light;
using namespace Rendering::Shadow;
using namespace Rendering::Buffer;
using namespace Rendering::Manager;
using namespace Rendering::GI;
using namespace Rendering::Shader;
using namespace Rendering::Factory;

void InjectRadianceFromSpotLight::Initialize(DirectX& dx, ShaderManager& shaderMgr, uint dimension)
{
	EngineShaderFactory factory(&shaderMgr);
	_shader = *factory.LoadComputeShader(dx, "InjectRadianceFromSpotLight", "CS", nullptr, "@InjectRadianceFromSpotLight");

	uint xzLength = InjectRadianceFormUtility::CalcThreadSideLength(dimension);
	_shader.SetThreadGroupInfo(ComputeShader::ThreadGroup(xzLength, xzLength, xzLength));
}

void InjectRadianceFromSpotLight::Inject(DirectX& dx, LightManager& lightMgr, ShadowSystemParam& shadowSystem, InjectRadianceFormUtility::BindParam& bindParam)
{
	auto& slBuffer = lightMgr.GetBuffer<SpotLight>();
	ComputeShader::BindShaderResourceView(dx, TextureBindIndex::SpotLightRadiusWithCenter, slBuffer.GetTransformSRBuffer().GetShaderResourceView());
	ComputeShader::BindShaderResourceView(dx, TextureBindIndex::SpotLightColor, slBuffer.GetColorSRBuffer().GetShaderResourceView());
	ComputeShader::BindShaderResourceView(dx, TextureBindIndex::SpotLightOptionalParamIndex, slBuffer.GetOptionalParamIndexSRBuffer().GetShaderResourceView());

	auto& slsBuffer = shadowSystem.manager.GetBuffer<SpotLightShadow>().GetBuffer();
	ComputeShader::BindShaderResourceView(dx, TextureBindIndex::SpotLightShadowParam, slsBuffer.GetParamSRBuffer().GetShaderResourceView());
	ComputeShader::BindShaderResourceView(dx, TextureBindIndex::SpotLightShadowViewProjMatrix, slsBuffer.GetViewProjMatSRBuffer().GetShaderResourceView());
	ComputeShader::BindShaderResourceView(dx, TextureBindIndex::SpotLightShadowMapAtlas, shadowSystem.renderer.GetShadowAtlasMap<SpotLightShadow>().GetDepthBuffer().GetTexture2D().GetShaderResourceView());

	InjectRadianceFormUtility::Bind(dx, bindParam);

	_shader.Dispatch(dx);

	InjectRadianceFormUtility::UnBind(dx);

	ComputeShader::UnBindShaderResourceView(dx, TextureBindIndex::SpotLightRadiusWithCenter);
	ComputeShader::UnBindShaderResourceView(dx, TextureBindIndex::SpotLightColor);
	ComputeShader::UnBindShaderResourceView(dx, TextureBindIndex::SpotLightShadowParam);
	ComputeShader::UnBindShaderResourceView(dx, TextureBindIndex::SpotLightOptionalParamIndex);
	ComputeShader::UnBindShaderResourceView(dx, TextureBindIndex::SpotLightShadowViewProjMatrix);
	ComputeShader::UnBindShaderResourceView(dx, TextureBindIndex::SpotLightShadowMapAtlas);
}