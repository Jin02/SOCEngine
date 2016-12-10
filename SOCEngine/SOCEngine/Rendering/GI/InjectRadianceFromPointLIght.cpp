#include "InjectRadianceFromPointLIght.h"
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

InjectRadianceFromPointLIght::InjectRadianceFromPointLIght() : InjectRadiance()
{
}

InjectRadianceFromPointLIght::~InjectRadianceFromPointLIght()
{
}

void InjectRadianceFromPointLIght::Initialize()
{
	InjectRadiance::Initialize("InjectRadianceFromPointLight");
}

void InjectRadianceFromPointLIght::Inject(const Device::DirectX*& dx, const LightManager* lightMgr, const ShadowRenderer*& shadowMgr, const InjectRadiance::DispatchParam& param)
{
	uint xzLength = (param.dimension + INJECTION_TILE_RES - 1) / INJECTION_TILE_RES;
	ComputeShader::ThreadGroup threadGroup(xzLength, xzLength, xzLength);
	
	_shader->SetThreadGroupInfo(threadGroup);

	ID3D11DeviceContext* context = dx->GetContext();

	ComputeShader::BindShaderResourceBuffer(context,	TextureBindIndex::PointLightRadiusWithCenter,		lightMgr->GetPointLightTransformSRBuffer());
	ComputeShader::BindShaderResourceBuffer(context,	TextureBindIndex::PointLightColor,					lightMgr->GetPointLightColorSRBuffer());
	ComputeShader::BindShaderResourceBuffer(context,	TextureBindIndex::PointLightShadowParam,			shadowMgr->GetPointLightShadowParamSRBuffer());
	ComputeShader::BindShaderResourceBuffer(context,	TextureBindIndex::PointLightOptionalParamIndex,		lightMgr->GetPointLightOptionalParamIndexSRBuffer());
	ComputeShader::BindShaderResourceBuffer(context,	TextureBindIndex::PointLightShadowViewProjMatrix,	shadowMgr->GetPointLightShadowViewProjSRBuffer());
	ComputeShader::BindTexture(context,					TextureBindIndex::PointLightShadowMapAtlas,			shadowMgr->GetPointLightShadowMapAtlas());

	Dispath(dx, param);

	ComputeShader::BindShaderResourceBuffer(context,	TextureBindIndex::PointLightRadiusWithCenter,		nullptr);
	ComputeShader::BindShaderResourceBuffer(context,	TextureBindIndex::PointLightColor,					nullptr);
	ComputeShader::BindShaderResourceBuffer(context,	TextureBindIndex::PointLightShadowParam,			nullptr);
	ComputeShader::BindShaderResourceBuffer(context,	TextureBindIndex::PointLightOptionalParamIndex,		nullptr);
	ComputeShader::BindShaderResourceBuffer(context,	TextureBindIndex::PointLightShadowViewProjMatrix,	nullptr);
	ComputeShader::BindTexture(context,					TextureBindIndex::PointLightShadowMapAtlas,			nullptr);
}