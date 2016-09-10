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

void InjectRadianceFromPointLIght::Initialize(const InjectRadiance::InitParam& initParam)
{
	const Scene* curScene			= Director::SharedInstance()->GetCurrentScene();
	const LightManager* lightMgr	= curScene->GetLightManager();
	const ShadowRenderer* shadowMgr	= curScene->GetShadowManager();

	InjectRadiance::Initialize("InjectRadianceFromPointLight", initParam);

	std::vector<ShaderForm::InputShaderResourceBuffer> inputSRBuffers = _shader->GetInputSRBuffers();
	{
		inputSRBuffers.push_back(ShaderForm::InputShaderResourceBuffer(uint(TextureBindIndex::PointLightRadiusWithCenter),		lightMgr->GetPointLightTransformSRBuffer()));
		inputSRBuffers.push_back(ShaderForm::InputShaderResourceBuffer(uint(TextureBindIndex::PointLightColor),					lightMgr->GetPointLightColorSRBuffer()));

		inputSRBuffers.push_back(ShaderForm::InputShaderResourceBuffer(uint(TextureBindIndex::PointLightShadowParam),			shadowMgr->GetPointLightShadowParamSRBuffer()));
		inputSRBuffers.push_back(ShaderForm::InputShaderResourceBuffer(uint(TextureBindIndex::PointLightShadowInvVPVMat),		shadowMgr->GetPointLightInvViewProjViewpotSRBuffer()));
		inputSRBuffers.push_back(ShaderForm::InputShaderResourceBuffer(uint(TextureBindIndex::PointLightShadowIndex),			lightMgr->GetPointLightShadowIndexSRBuffer()));
		inputSRBuffers.push_back(ShaderForm::InputShaderResourceBuffer(uint(TextureBindIndex::PointLightShadowViewProjMatrix),	shadowMgr->GetPointLightShadowViewProjSRBuffer()));
	}
	_shader->SetInputSRBuffers(inputSRBuffers);

	std::vector<ShaderForm::InputTexture> inputTextures = _shader->GetInputTextures();
	{
		inputTextures.push_back(ShaderForm::InputTexture( uint(TextureBindIndex::PointLightShadowMapAtlas), shadowMgr->GetPointLightShadowMapAtlas()) );
	}
	_shader->SetInputTextures(inputTextures);
}

void InjectRadianceFromPointLIght::Inject(const Device::DirectX*& dx, const ShadowRenderer*& shadowMgr, const Voxelization* voxelization, uint dimension, uint maximumCascade)
{
	uint xzLength = (dimension + INJECTION_TILE_RES - 1) / INJECTION_TILE_RES;

	ComputeShader::ThreadGroup threadGroup(
		xzLength,
		(dimension * maximumCascade + INJECTION_TILE_RES - 1) / INJECTION_TILE_RES,
		xzLength );
	
	_shader->SetThreadGroupInfo(threadGroup);
	Dispath(dx, voxelization->GetConstBuffers());
}