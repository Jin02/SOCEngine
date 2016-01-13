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

void InjectRadianceFromDirectionalLIght::Initialize(const ConstBuffer*& giInfoConstBuffer)
{
	const Scene* curScene			= Director::SharedInstance()->GetCurrentScene();
	const LightManager* lightMgr	= curScene->GetLightManager();
	const ShadowRenderer* shadowMgr	= curScene->GetShadowManager();

	ComputeShader::ThreadGroup threadGroup;
	InjectRadiance::Initialize("InjectRadianceFromDirectionalLight", threadGroup, giInfoConstBuffer);

	std::vector<ShaderForm::InputShaderResourceBuffer> inputSRBuffers = _shader->GetInputSRBuffers();
	{
		//inputSRBuffers.push_back(ShaderForm::InputShaderResourceBuffer(uint(TextureBindIndex::DirectionalLightCenterWithDirZ),			lightMgr->GetDirectionalLightTransformSRBuffer()));
		//inputSRBuffers.push_back(ShaderForm::InputShaderResourceBuffer(uint(TextureBindIndex::DirectionalLightColor),					lightMgr->GetDirectionalLightColorSRBuffer()));
		//inputSRBuffers.push_back(ShaderForm::InputShaderResourceBuffer(uint(TextureBindIndex::DirectionalLightParam),					lightMgr->GetDirectionalLightParamSRBuffer()));
		//inputSRBuffers.push_back(ShaderForm::InputShaderResourceBuffer(uint(TextureBindIndex::DirectionalLightShadowParam),				lightMgr->GetDirectionalLightShadowParamSRBuffer()));
		//inputSRBuffers.push_back(ShaderForm::InputShaderResourceBuffer(uint(TextureBindIndex::DirectionalLightShadowColor),				lightMgr->GetDirectionalLightShadowColorSRBuffer()));
		//inputSRBuffers.push_back(ShaderForm::InputShaderResourceBuffer(uint(TextureBindIndex::DirectionalLightShadowIndexToLightIndex),	shadowMgr->GetDirectionalLightShadowIndexToLightIndexSRBuffer()));
		//inputSRBuffers.push_back(ShaderForm::InputShaderResourceBuffer(uint(TextureBindIndex::DirectionalLightShadowInvVPVMat),			nullptr));
	}
	_shader->SetInputSRBuffers(inputSRBuffers);

	std::vector<ShaderForm::InputTexture> inputTextures;
	{
		inputTextures.push_back(ShaderForm::InputTexture( uint(TextureBindIndex::DirectionalLightShadowMapAtlas), shadowMgr->GetDirectionalLightShadowMapAtlas()) );
	}
	_shader->SetInputTextures(inputTextures);
}