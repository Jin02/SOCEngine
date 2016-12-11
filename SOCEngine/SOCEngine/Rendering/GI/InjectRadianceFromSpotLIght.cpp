#include "InjectRadianceFromSpotLIght.h"
#include "BindIndexInfo.h"
#include "Director.h"
#include "Size.h"

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
using namespace GPGPU::DirectCompute;

InjectRadianceFromSpotLIght::InjectRadianceFromSpotLIght() : InjectRadiance()
{
}

InjectRadianceFromSpotLIght::~InjectRadianceFromSpotLIght()
{
}

void InjectRadianceFromSpotLIght::Initialize()
{
	InjectRadiance::Initialize("InjectRadianceFromSpotLight");
}

void InjectRadianceFromSpotLIght::Inject(const Device::DirectX*& dx, const Manager::LightManager* lightMgr, const Shadow::ShadowRenderer*& shadowMgr, const InjectRadiance::DispatchParam& param)
{
	uint xzLength = (param.dimension + INJECTION_TILE_RES - 1) / INJECTION_TILE_RES;
	ComputeShader::ThreadGroup threadGroup(xzLength, xzLength, xzLength);
	
	_shader->SetThreadGroupInfo(threadGroup);

	ID3D11DeviceContext* context = dx->GetContext();

	ComputeShader::BindShaderResourceBuffer(context,	TextureBindIndex::SpotLightRadiusWithCenter,		lightMgr->GetSpotLightTransformSRBuffer());
	ComputeShader::BindShaderResourceBuffer(context,	TextureBindIndex::SpotLightColor,					lightMgr->GetSpotLightColorSRBuffer());
	ComputeShader::BindShaderResourceBuffer(context,	TextureBindIndex::SpotLightShadowParam,				shadowMgr->GetSpotLightShadowParamSRBuffer());
	ComputeShader::BindShaderResourceBuffer(context,	TextureBindIndex::SpotLightOptionalParamIndex,				lightMgr->GetSpotLightOptionalParamIndexSRBuffer());
	ComputeShader::BindShaderResourceBuffer(context,	TextureBindIndex::SpotLightShadowViewProjMatrix,	shadowMgr->GetSpotLightShadowViewProjSRBuffer());
	ComputeShader::BindTexture(context,					TextureBindIndex::SpotLightShadowMapAtlas,			shadowMgr->GetSpotLightShadowMapAtlas());
	ComputeShader::BindSamplerState(context,			SamplerStateBindIndex::VSMShadowSamplerState,		dx->GetShadowSamplerState());

	Dispath(dx, param);

	ComputeShader::BindSamplerState(context,			SamplerStateBindIndex::VSMShadowSamplerState,		nullptr);
	ComputeShader::BindShaderResourceBuffer(context,	TextureBindIndex::SpotLightRadiusWithCenter,		nullptr);
	ComputeShader::BindShaderResourceBuffer(context,	TextureBindIndex::SpotLightColor,					nullptr);
	ComputeShader::BindShaderResourceBuffer(context,	TextureBindIndex::SpotLightShadowParam,				nullptr);
	ComputeShader::BindShaderResourceBuffer(context,	TextureBindIndex::SpotLightOptionalParamIndex,		nullptr);
	ComputeShader::BindShaderResourceBuffer(context,	TextureBindIndex::SpotLightShadowViewProjMatrix,	nullptr);
	ComputeShader::BindTexture(context,					TextureBindIndex::SpotLightShadowMapAtlas,			nullptr);
}