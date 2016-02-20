#include "InjectRadianceFromDirectionalLIght.h"
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

InjectRadianceFromDirectionalLIght::InjectRadianceFromDirectionalLIght() : InjectRadiance()
{
}

InjectRadianceFromDirectionalLIght::~InjectRadianceFromDirectionalLIght()
{
}

void InjectRadianceFromDirectionalLIght::Initialize(const InjectRadiance::InitParam& initParam)
{
	const Scene* curScene			= Director::SharedInstance()->GetCurrentScene();
	const LightManager* lightMgr	= curScene->GetLightManager();
	const ShadowRenderer* shadowMgr	= curScene->GetShadowManager();

	InjectRadiance::Initialize("InjectRadianceFromDirectionalLight", initParam);

	std::vector<ShaderForm::InputShaderResourceBuffer> inputSRBuffers = _shader->GetInputSRBuffers();
	{
		inputSRBuffers.push_back(ShaderForm::InputShaderResourceBuffer(uint(TextureBindIndex::DirectionalLightCenterWithDirZ),			lightMgr->GetDirectionalLightTransformSRBuffer()));
		inputSRBuffers.push_back(ShaderForm::InputShaderResourceBuffer(uint(TextureBindIndex::DirectionalLightColor),					lightMgr->GetDirectionalLightColorSRBuffer()));
		inputSRBuffers.push_back(ShaderForm::InputShaderResourceBuffer(uint(TextureBindIndex::DirectionalLightParam),					lightMgr->GetDirectionalLightParamSRBuffer()));

		inputSRBuffers.push_back(ShaderForm::InputShaderResourceBuffer(uint(TextureBindIndex::DirectionalLightShadowParam),				shadowMgr->GetDirectionalLightShadowParamSRBuffer()));
		inputSRBuffers.push_back(ShaderForm::InputShaderResourceBuffer(uint(TextureBindIndex::DirectionalLightShadowInvVPVMat),			shadowMgr->GetDirectionalLightInvViewProjViewpotSRBuffer()));
		inputSRBuffers.push_back(ShaderForm::InputShaderResourceBuffer(uint(TextureBindIndex::DirectionalLightShadowIndex),				lightMgr->GetDirectionalLightShadowIndexSRBuffer()));
		inputSRBuffers.push_back(ShaderForm::InputShaderResourceBuffer(uint(TextureBindIndex::DirectionalLightShadowViewProjMatrix),	shadowMgr->GetDirectionalLightShadowViewProjSRBuffer()));
	}
	_shader->SetInputSRBuffers(inputSRBuffers);

	std::vector<ShaderForm::InputTexture> inputTextures = _shader->GetInputTextures();
	{
		inputTextures.push_back(ShaderForm::InputTexture( uint(TextureBindIndex::DirectionalLightShadowMapAtlas), shadowMgr->GetDirectionalLightShadowMapAtlas()) );
	}
	_shader->SetInputTextures(inputTextures);
}

void InjectRadianceFromDirectionalLIght::Inject(const Device::DirectX*& dx, const ShadowRenderer*& shadowMgr, const Voxelization* voxelization)
{
	Size<uint> activatedShadowMapSize	= shadowMgr->GetActivatedDLShadowMapSize();

	ComputeShader::ThreadGroup threadGroup(	(activatedShadowMapSize.w + INJECTION_TILE_RES - 1) / INJECTION_TILE_RES,
											(activatedShadowMapSize.h + INJECTION_TILE_RES - 1) / INJECTION_TILE_RES, 1 );

	_shader->SetThreadGroupInfo(threadGroup);
	Dispath(dx, voxelization->GetConstBuffers());
}