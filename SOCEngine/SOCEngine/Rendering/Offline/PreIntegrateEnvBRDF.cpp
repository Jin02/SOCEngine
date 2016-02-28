#include "PreIntegrateEnvBRDF.h"
#include "EngineShaderFactory.hpp"
#include "ShaderManager.h"
#include "ResourceManager.h"
#include "Director.h"
#include "ComputeShader.h"
#include "DirectX.h"

using namespace Math;
using namespace Device;
using namespace Core;
using namespace Resource;
using namespace Rendering;
using namespace Rendering::Offline;
using namespace Rendering::Shader;
using namespace Rendering::Manager;
using namespace Rendering::Factory;
using namespace Rendering::Texture;
using namespace GPGPU::DirectCompute;

PreIntegrateEnvBRDF::PreIntegrateEnvBRDF() : _map(nullptr)
{
}

PreIntegrateEnvBRDF::~PreIntegrateEnvBRDF()
{
	SAFE_DELETE(_map);
}

Texture::Texture2D* PreIntegrateEnvBRDF::CreatePreBRDFMap()
{
	auto LoadComputeShader = [](const std::string& fileName) -> ComputeShader*
	{
		ComputeShader* shader = nullptr;

		std::string filePath = "";
		EngineFactory pathFinder(nullptr);
		pathFinder.FetchShaderFullPath(filePath, fileName);
	
		ASSERT_COND_MSG(filePath.empty() == false, "Error, File path is empty!");
	
		ResourceManager* resourceManager = ResourceManager::SharedInstance();
		auto shaderMgr = resourceManager->GetShaderManager();
	
		ID3DBlob* blob = shaderMgr->CreateBlob(filePath, "cs", "CS", false, nullptr);
		shader = new ComputeShader(ComputeShader::ThreadGroup(0, 0, 0), blob);
		ASSERT_COND_MSG(shader->Initialize(), "can not create compute shader");

		return shader;
	};

	ComputeShader* shader = LoadComputeShader("PreIntegrateEnvBRDF");

	const Size<uint> size(256, 256);

	Texture2D* tex = new Texture2D;
	tex->Initialize(size.w, size.h, DXGI_FORMAT_R16G16_FLOAT, DXGI_FORMAT_R16G16_FLOAT, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS, 1, 1);

	std::vector<ShaderForm::InputUnorderedAccessView> inputUAVs;
	{
		const View::UnorderedAccessView* uav = tex->GetUnorderedAccessView();
		inputUAVs.push_back( ShaderForm::InputUnorderedAccessView(0, uav) );
	}
	shader->SetUAVs(inputUAVs);

	ComputeShader::ThreadGroup threadGroup(0, 0, 1);
	{
		threadGroup.x = (size.w + PRE_INTEGRATE_TILE_RES - 1) / PRE_INTEGRATE_TILE_RES;
		threadGroup.y = (size.h + PRE_INTEGRATE_TILE_RES - 1) / PRE_INTEGRATE_TILE_RES;
	}
	shader->SetThreadGroupInfo(threadGroup);
	shader->Dispatch(Director::SharedInstance()->GetDirectX()->GetContext());

	SAFE_DELETE(shader);

	return tex;
}

Texture::Texture2D* PreIntegrateEnvBRDF::FetchPreBRDFMap()
{
	if(_map)
		return _map;

	_map = CreatePreBRDFMap();
	return _map;
}