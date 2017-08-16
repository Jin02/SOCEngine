#include "InjectRadianceFromPointLight.h"
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

void InjectRadianceFromPointLight::Initialize(DirectX& dx, ShaderManager& shaderMgr, uint dimension)
{
	EngineShaderFactory factory(&shaderMgr);
	_shader = *factory.LoadComputeShader(dx, "InjectRadianceFromPointLight", "CS", nullptr, "@InjectRadianceFromPointLight");

	uint xzLength = InjectRadianceFormUtility::CalcThreadSideLength(dimension);
	_shader.SetThreadGroupInfo(ComputeShader::ThreadGroup(xzLength, xzLength, xzLength));
}

void InjectRadianceFromPointLight::Inject(DirectX& dx, LightManager& lightMgr, ShadowSystemParam& shadowSystem, InjectRadianceFormUtility::BindParam& bindParam)
{
	auto& plBuffer = lightMgr.GetBuffer<PointLight>();
	ComputeShader::BindShaderResourceView(dx, TextureBindIndex::PointLightRadiusWithCenter, plBuffer.GetTransformSRBuffer().GetShaderResourceView());
	ComputeShader::BindShaderResourceView(dx, TextureBindIndex::PointLightColor, plBuffer.GetColorSRBuffer().GetShaderResourceView());
	ComputeShader::BindShaderResourceView(dx, TextureBindIndex::PointLightOptionalParamIndex, plBuffer.GetOptionalParamIndexSRBuffer().GetShaderResourceView());

	auto& plsBuffer = shadowSystem.manager.GetBuffer<PointLightShadow>().GetBuffer();
	ComputeShader::BindShaderResourceView(dx, TextureBindIndex::PointLightShadowParam, plsBuffer.GetParamSRBuffer().GetShaderResourceView());
	ComputeShader::BindShaderResourceView(dx, TextureBindIndex::PointLightShadowViewProjMatrix, plsBuffer.GetViewProjMatSRBuffer().GetShaderResourceView());
	ComputeShader::BindShaderResourceView(dx, TextureBindIndex::PointLightShadowMapAtlas, shadowSystem.renderer.GetShadowAtlasMap<PointLightShadow>().GetTexture2D().GetShaderResourceView());

	InjectRadianceFormUtility::Bind(dx, bindParam);

	_shader.Dispatch(dx);

	InjectRadianceFormUtility::UnBind(dx);

	ComputeShader::UnBindShaderResourceView(dx, TextureBindIndex::PointLightRadiusWithCenter);
	ComputeShader::UnBindShaderResourceView(dx, TextureBindIndex::PointLightColor);
	ComputeShader::UnBindShaderResourceView(dx, TextureBindIndex::PointLightShadowParam);
	ComputeShader::UnBindShaderResourceView(dx, TextureBindIndex::PointLightOptionalParamIndex);
	ComputeShader::UnBindShaderResourceView(dx, TextureBindIndex::PointLightShadowViewProjMatrix);
	ComputeShader::UnBindShaderResourceView(dx, TextureBindIndex::PointLightShadowMapAtlas);
}