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

InjectRadiance::InjectRadiance()
	: _shader(nullptr)
{
}

InjectRadiance::~InjectRadiance()
{
	SAFE_DELETE(_shader);
}

void InjectRadiance::Initialize(const std::string& fileName,
								const ComputeShader::ThreadGroup& threadGroup,
								const ConstBuffer*& giInfoConstBuffer)
{
	std::string filePath = "";
	EngineFactory pathFinder(nullptr);
	pathFinder.FetchShaderFullPath(filePath, fileName);

	ASSERT_COND_MSG(filePath.empty() == false, "Error, File path is empty!");

	ResourceManager* resourceManager = ResourceManager::SharedInstance();
	auto shaderMgr = resourceManager->GetShaderManager();

	ID3DBlob* blob = shaderMgr->CreateBlob(filePath, "cs", "CS", false, nullptr);
	_shader = new ComputeShader(threadGroup, blob);
	ASSERT_COND_MSG(_shader->Initialize(), "can not create compute shader");

	// Setting Inputs
	{
		const Scene* curScene			= Director::SharedInstance()->GetCurrentScene();
		const ShadowRenderer* shadowMgr	= curScene->GetShadowManager();

		std::vector<ShaderForm::InputConstBuffer> inputConstBuffers;
		{
			inputConstBuffers.push_back(ShaderForm::InputConstBuffer(uint(ConstBufferBindIndex::ShadowGlobalParam),				shadowMgr->GetShadowGlobalParamConstBuffer()));
			inputConstBuffers.push_back(ShaderForm::InputConstBuffer(uint(ConstBufferBindIndex::GlobalIIllumination_InfoCB),	giInfoConstBuffer));
		}
		_shader->SetInputConstBuffers(inputConstBuffers);
	}
}

void InjectRadiance::Destroy()
{
	
}