#include "InjectRadianceFromPointLight.h"
#include "BindIndexInfo.h"
#include "ShaderFactory.hpp"

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
using namespace Rendering::Renderer;

void InjectRadianceFromPointLight::Initialize(DirectX& dx, ShaderManager& shaderMgr, uint dimension)
{
	_shader			= *ShaderFactory::LoadComputeShader(dx, shaderMgr, "InjectRadianceFromPointLight", "CS", nullptr, "@InjectRadianceFromPointLight");
	_threadLength	= InjectRadianceFormUtility::CalcThreadSideLength(dimension);
}

void InjectRadianceFromPointLight::Inject(	DirectX& dx, VoxelMap& outVoxelMap,
											const LightManager& lightMgr,
											const ShadowSystem& shadowParam,
											const InjectRadianceFormUtility::BindParam& bindParam	)
{
	auto& plBuffer = lightMgr.GetBuffer<PointLight>();
	ComputeShader::BindShaderResourceView(dx, TextureBindIndex::PointLightRadiusWithCenter,		plBuffer.GetTransformSRBuffer().GetShaderResourceView());
	ComputeShader::BindShaderResourceView(dx, TextureBindIndex::PointLightColor,				plBuffer.GetColorSRBuffer().GetShaderResourceView());
	ComputeShader::BindShaderResourceView(dx, TextureBindIndex::PointLightOptionalParamIndex,	plBuffer.GetOptionalParamIndexSRBuffer().GetShaderResourceView());

	auto& plsBuffer = shadowParam.manager.GetBuffer<PointLightShadow>();
	ComputeShader::BindShaderResourceView(dx, TextureBindIndex::PointLightShadowParam,			plsBuffer.GetParamSRBuffer().GetShaderResourceView());
	ComputeShader::BindShaderResourceView(dx, TextureBindIndex::PointLightShadowViewProjMatrix,	plsBuffer.GetViewProjMatSRBuffer().GetShaderResourceView());
	ComputeShader::BindShaderResourceView(dx, TextureBindIndex::PointLightShadowMapAtlas,		shadowParam.renderer.GetShadowAtlasMap<PointLightShadow>().GetTexture2D().GetShaderResourceView());

	InjectRadianceFormUtility::Bind(dx, outVoxelMap, bindParam);

	_shader.Dispatch(dx, {_threadLength, _threadLength, _threadLength});

	InjectRadianceFormUtility::UnBind(dx);

	ComputeShader::UnBindShaderResourceView(dx, TextureBindIndex::PointLightRadiusWithCenter);
	ComputeShader::UnBindShaderResourceView(dx, TextureBindIndex::PointLightColor);
	ComputeShader::UnBindShaderResourceView(dx, TextureBindIndex::PointLightShadowParam);
	ComputeShader::UnBindShaderResourceView(dx, TextureBindIndex::PointLightOptionalParamIndex);
	ComputeShader::UnBindShaderResourceView(dx, TextureBindIndex::PointLightShadowViewProjMatrix);
	ComputeShader::UnBindShaderResourceView(dx, TextureBindIndex::PointLightShadowMapAtlas);
}