#include "InjectRadianceFromDirectionalLIght.h"
#include "BindIndexInfo.h"
#include "Director.h"

using namespace Device;
using namespace Core;
using namespace Rendering;
using namespace Rendering::Light;
using namespace Rendering::Shadow;
using namespace Rendering::Buffer;
using namespace Rendering::Manager;
using namespace Rendering::GI;
using namespace Rendering::Shader;
using namespace GPGPU::DirectCompute;

InjectRadianceFromDirectionalLIght::InjectRadianceFromDirectionalLIght()
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

	ComputeShader::ThreadGroup threadGroup;
	InjectRadiance::Initialize("InjectRadianceFromDirectionalLight", threadGroup, initParam);

	std::vector<ShaderForm::InputShaderResourceBuffer> inputSRBuffers = _shader->GetInputSRBuffers();
	{
		inputSRBuffers.push_back(ShaderForm::InputShaderResourceBuffer(uint(TextureBindIndex::DirectionalLightCenterWithDirZ),	lightMgr->GetDirectionalLightTransformSRBuffer()));
		inputSRBuffers.push_back(ShaderForm::InputShaderResourceBuffer(uint(TextureBindIndex::DirectionalLightColor),			lightMgr->GetDirectionalLightColorSRBuffer()));
		inputSRBuffers.push_back(ShaderForm::InputShaderResourceBuffer(uint(TextureBindIndex::DirectionalLightParam),			lightMgr->GetDirectionalLightParamSRBuffer()));

		inputSRBuffers.push_back(ShaderForm::InputShaderResourceBuffer(uint(TextureBindIndex::DirectionalLightShadowParam),		shadowMgr->GetDirectionalLightShadowParamSRBuffer()));
		inputSRBuffers.push_back(ShaderForm::InputShaderResourceBuffer(uint(TextureBindIndex::DirectionalLightShadowInvVPVMat),	shadowMgr->GetDirectionalLightInvViewProjViewpotSRBuffer()));
	}
	_shader->SetInputSRBuffers(inputSRBuffers);

	std::vector<ShaderForm::InputTexture> inputTextures = _shader->GetInputTextures();
	{
		inputTextures.push_back(ShaderForm::InputTexture( uint(TextureBindIndex::DirectionalLightShadowMapAtlas), shadowMgr->GetDirectionalLightShadowMapAtlas()) );
	}
	_shader->SetInputTextures(inputTextures);
}