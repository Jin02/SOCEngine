#include "InjectRadiance.h"
#include "EngineShaderFactory.hpp"
#include "ShaderManager.h"
#include "ResourceManager.h"
#include "Director.h"

using namespace Device;
using namespace Core;
using namespace Rendering;
using namespace Rendering::Shadow;
using namespace Rendering::Factory;
using namespace Rendering::Manager;
using namespace Rendering::Buffer;
using namespace Rendering::GI;
using namespace Rendering::Shader;
using namespace GPGPU::DirectCompute;
using namespace Resource;

InjectRadiance::InjectRadiance() : _shader(nullptr)
{
}

InjectRadiance::~InjectRadiance()
{
	SAFE_DELETE(_shader);
}

void InjectRadiance::Initialize(const std::string& fileName, const InitParam& param)
{
	ASSERT_COND_MSG(param.IsValid(),		"Error,  Param is invalid.");

	std::string filePath = "";
	EngineFactory pathFinder(nullptr);
	pathFinder.FetchShaderFullPath(filePath, fileName);

	ASSERT_COND_MSG(filePath.empty() == false, "Error, File path is empty!");

	ResourceManager* resourceManager = ResourceManager::SharedInstance();
	auto shaderMgr = resourceManager->GetShaderManager();

	ID3DBlob* blob = shaderMgr->CreateBlob(filePath, "cs", "CS", false, nullptr);
	_shader = new ComputeShader(ComputeShader::ThreadGroup(0, 0, 0), blob);
	ASSERT_COND_MSG(_shader->Initialize(), "can not create compute shader");

	// Setting Inputs
	{
		const Scene* curScene			= Director::SharedInstance()->GetCurrentScene();
		const ShadowRenderer* shadowMgr	= curScene->GetShadowManager();

		std::vector<ShaderForm::InputConstBuffer> inputConstBuffers;
		{
			inputConstBuffers.push_back(ShaderForm::InputConstBuffer(uint(ConstBufferBindIndex::ShadowGlobalParam),				shadowMgr->GetShadowGlobalParamConstBuffer()));
			inputConstBuffers.push_back(ShaderForm::InputConstBuffer(uint(ConstBufferBindIndex::GI_GlobalStaticInfoCB),			param.giStaticInfoCB));
			inputConstBuffers.push_back(ShaderForm::InputConstBuffer(uint(ConstBufferBindIndex::GI_GlobalDynamicInfoCB),		param.giDynamicInfoCB));
		}
		_shader->SetInputConstBuffers(inputConstBuffers);

		std::vector<ShaderForm::InputRAWBuffer> inputRawBuffers;
		{
			inputRawBuffers.push_back(ShaderForm::InputRAWBuffer(uint(TextureBindIndex::AnisotropicVoxelAlbedoTexture),				param.voxelization->GetAnisotropicVoxelAlbedoMapAtlas()));
			inputRawBuffers.push_back(ShaderForm::InputRAWBuffer(uint(TextureBindIndex::AnisotropicVoxelNormalTexture),				param.voxelization->GetAnisotropicVoxelNormalMapAtlas()));
			inputRawBuffers.push_back(ShaderForm::InputRAWBuffer(uint(TextureBindIndex::AnisotropicVoxelEmissionTexture),			param.voxelization->GetAnisotropicVoxelEmissionMapAtlas()));
		}
		_shader->SetInputRAWBuffers(inputRawBuffers);
	}

	uint dimension = param.dimension;

	// Setting Output
	{
		std::vector<ShaderForm::InputUnorderedAccessView> outputs;
		{
			outputs.push_back(ShaderForm::InputUnorderedAccessView(uint(UAVBindIndex::OutAnisotropicVoxelColorTexture),			param.outColorMap->GetSourceMapUAV()));
		}

		_shader->SetUAVs(outputs);
	}
}

void InjectRadiance::Dispath(const Device::DirectX* dx, const std::vector<Buffer::ConstBuffer*>& voxelizationInfoConstBuffers)
{
	ID3D11DeviceContext* context = dx->GetContext();
	ComputeShader::BindSamplerState(context, SamplerStateBindIndex::ShadowComprisonSamplerState, dx->GetShadowGreaterEqualSamplerComparisonState());

	for(auto iter = voxelizationInfoConstBuffers.begin(); iter != voxelizationInfoConstBuffers.end(); ++iter)
	{
		ComputeShader::BindConstBuffer(context, ConstBufferBindIndex::Voxelization_InfoCB, (*iter));
		_shader->Dispatch(context);
	}

	ComputeShader::BindSamplerState(context, SamplerStateBindIndex::ShadowComprisonSamplerState, nullptr);
	ComputeShader::BindConstBuffer(context, ConstBufferBindIndex::Voxelization_InfoCB, nullptr);
}

void InjectRadiance::Destroy()
{
}
