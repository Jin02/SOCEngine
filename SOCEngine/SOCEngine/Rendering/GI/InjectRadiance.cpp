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

	std::vector<ShaderMacro> macros;
	{
		// 무조건 일반 Shadow Map을 사용해야하기 때문에 아래 매크로를 추가해준다.
		macros.push_back(ShaderMacro("NEVER_USE_VSM", ""));
	}

	ID3DBlob* blob = shaderMgr->CreateBlob(filePath, "cs", "CS", false, &macros);
	_shader = new ComputeShader(ComputeShader::ThreadGroup(0, 0, 0), blob);
	ASSERT_COND_MSG(_shader->Initialize(), "can not create compute shader");

	// Setting Inputs
	{
		const Scene* curScene			= Director::SharedInstance()->GetCurrentScene();
		const ShadowRenderer* shadowMgr	= curScene->GetShadowManager();

		std::vector<ShaderForm::InputConstBuffer> inputConstBuffers;
		{
			inputConstBuffers.push_back(ShaderForm::InputConstBuffer(uint(ConstBufferBindIndex::ShadowGlobalParam),				shadowMgr->GetShadowGlobalParamConstBuffer()));
			inputConstBuffers.push_back(ShaderForm::InputConstBuffer(uint(ConstBufferBindIndex::GlobalIIllumination_InfoCB),	param.giInfoConstBuffer));
		}
		_shader->SetInputConstBuffers(inputConstBuffers);

		std::vector<ShaderForm::InputTexture> inputTextures;
		{
			inputTextures.push_back(ShaderForm::InputTexture(uint(TextureBindIndex::AnisotropicVoxelAlbedoTexture),				param.voxelization->GetAnisotropicVoxelAlbedoMapAtlas()));
			inputTextures.push_back(ShaderForm::InputTexture(uint(TextureBindIndex::AnisotropicVoxelNormalTexture),				param.voxelization->GetAnisotropicVoxelNormalMapAtlas()));
			inputTextures.push_back(ShaderForm::InputTexture(uint(TextureBindIndex::AnisotropicVoxelEmissionTexture),			param.voxelization->GetAnisotropicVoxelEmissionMapAtlas()));
		}
		_shader->SetInputTextures(inputTextures);
	}

	uint dimension = 1 << param.globalInfo->voxelDimensionPow2;

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

	for(auto iter = voxelizationInfoConstBuffers.begin(); iter != voxelizationInfoConstBuffers.end(); ++iter)
	{
		ID3D11Buffer* cb = (*iter)->GetBuffer();
		context->CSSetConstantBuffers(uint(ConstBufferBindIndex::Voxelization_InfoCB), 1, &cb);
		_shader->Dispatch(context);
	}

	//ID3D11Buffer* cb = nullptr;
	//context->CSSetConstantBuffers(uint(ConstBufferBindIndex::Voxelization_InfoCB), 1, &cb);
}

void InjectRadiance::Destroy()
{
}