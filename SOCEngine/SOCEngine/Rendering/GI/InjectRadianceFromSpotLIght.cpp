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

void InjectRadianceFromSpotLIght::Initialize(const InjectRadiance::InitParam& initParam)
{
	const Scene* curScene			= Director::SharedInstance()->GetCurrentScene();
	const LightManager* lightMgr	= curScene->GetLightManager();
	const ShadowRenderer* shadowMgr	= curScene->GetShadowManager();

	InjectRadiance::Initialize("InjectRadianceFromSpotLight", initParam);

	std::vector<ShaderForm::InputShaderResourceBuffer> inputSRBuffers = _shader->GetInputSRBuffers();
	{
		inputSRBuffers.push_back(ShaderForm::InputShaderResourceBuffer(uint(TextureBindIndex::SpotLightRadiusWithCenter),		lightMgr->GetSpotLightTransformSRBuffer()));
		inputSRBuffers.push_back(ShaderForm::InputShaderResourceBuffer(uint(TextureBindIndex::SpotLightColor),					lightMgr->GetSpotLightColorSRBuffer()));
		inputSRBuffers.push_back(ShaderForm::InputShaderResourceBuffer(uint(TextureBindIndex::SpotLightParam),					lightMgr->GetSpotLightParamSRBuffer()));

		inputSRBuffers.push_back(ShaderForm::InputShaderResourceBuffer(uint(TextureBindIndex::SpotLightShadowParam),			shadowMgr->GetSpotLightShadowParamSRBuffer()));
		inputSRBuffers.push_back(ShaderForm::InputShaderResourceBuffer(uint(TextureBindIndex::SpotLightShadowInvVPVMat),		shadowMgr->GetSpotLightInvViewProjViewpotSRBuffer()));
		inputSRBuffers.push_back(ShaderForm::InputShaderResourceBuffer(uint(TextureBindIndex::SpotLightShadowIndex),			lightMgr->GetSpotLightShadowIndexSRBuffer()));
		inputSRBuffers.push_back(ShaderForm::InputShaderResourceBuffer(uint(TextureBindIndex::SpotLightShadowViewProjMatrix),	shadowMgr->GetSpotLightShadowViewProjSRBuffer()));
	}
	_shader->SetInputSRBuffers(inputSRBuffers);

	std::vector<ShaderForm::InputTexture> inputTextures = _shader->GetInputTextures();
	{
		inputTextures.push_back(ShaderForm::InputTexture( uint(TextureBindIndex::SpotLightShadowMapAtlas), shadowMgr->GetSpotLightShadowMapAtlas()) );
	}
	_shader->SetInputTextures(inputTextures);
}

void InjectRadianceFromSpotLIght::Inject(const Device::DirectX*& dx, const ShadowRenderer*& shadowMgr, const Voxelization* voxelization, uint dimension, uint maximumCascade)
{
	uint xzLength = (dimension + INJECTION_TILE_RES - 1) / INJECTION_TILE_RES;

	ComputeShader::ThreadGroup threadGroup(
		xzLength,
		(dimension * maximumCascade + INJECTION_TILE_RES - 1) / INJECTION_TILE_RES,
		xzLength );

	_shader->SetThreadGroupInfo(threadGroup);
	Dispath(dx, voxelization->GetConstBuffers());
}