#include "InjectRadianceFromSpotLight.h"
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

void InjectRadianceFromSpotLight::Initialize(DirectX& dx, ShaderManager& shaderMgr, uint dimension)
{
	_shader			= *ShaderFactory::LoadComputeShader(dx, shaderMgr, "InjectRadianceFromSpotLight", "CS", nullptr, "@InjectRadianceFromSpotLight");
	_threadLength	= InjectRadianceFormUtility::CalcThreadSideLength(dimension);
}

void InjectRadianceFromSpotLight::Inject(	DirectX& dx, VoxelMap& outVoxelMap,
											const LightManager& lightMgr,
											const ShadowSystem& shadowParam,
											const InjectRadianceFormUtility::BindParam& bindParam	)
{
	auto& slBuffer = lightMgr.GetBuffer<SpotLight>();
	ComputeShader::BindShaderResourceView(dx, TextureBindIndex::SpotLightRadiusWithCenter,		slBuffer.GetTransformSRBuffer().GetShaderResourceView());
	ComputeShader::BindShaderResourceView(dx, TextureBindIndex::SpotLightColor,					slBuffer.GetColorSRBuffer().GetShaderResourceView());
	ComputeShader::BindShaderResourceView(dx, TextureBindIndex::SpotLightOptionalParamIndex,	slBuffer.GetOptionalParamIndexSRBuffer().GetShaderResourceView());

	auto& slsBuffer = shadowParam.manager.GetBuffer<SpotLightShadow>();
	ComputeShader::BindShaderResourceView(dx, TextureBindIndex::SpotLightShadowParam,			slsBuffer.GetParamSRBuffer().GetShaderResourceView());
	ComputeShader::BindShaderResourceView(dx, TextureBindIndex::SpotLightShadowViewProjMatrix,	slsBuffer.GetViewProjMatSRBuffer().GetShaderResourceView());
	ComputeShader::BindShaderResourceView(dx, TextureBindIndex::SpotLightShadowMapAtlas,		shadowParam.renderer.GetShadowAtlasMap<SpotLightShadow>().GetTexture2D().GetShaderResourceView());

	InjectRadianceFormUtility::Bind(dx, outVoxelMap, bindParam);

	_shader.Dispatch(dx, {_threadLength, _threadLength, _threadLength});

	InjectRadianceFormUtility::UnBind(dx);

	ComputeShader::UnBindShaderResourceView(dx, TextureBindIndex::SpotLightRadiusWithCenter);
	ComputeShader::UnBindShaderResourceView(dx, TextureBindIndex::SpotLightColor);
	ComputeShader::UnBindShaderResourceView(dx, TextureBindIndex::SpotLightShadowParam);
	ComputeShader::UnBindShaderResourceView(dx, TextureBindIndex::SpotLightOptionalParamIndex);
	ComputeShader::UnBindShaderResourceView(dx, TextureBindIndex::SpotLightShadowViewProjMatrix);
	ComputeShader::UnBindShaderResourceView(dx, TextureBindIndex::SpotLightShadowMapAtlas);
}