#include "MipmapVoxelTexture.h"
#include "Utility.h"
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
using namespace Rendering::View;
using namespace Rendering::Shader;
using namespace GPGPU::DirectCompute;
using namespace Resource;

MipmapVoxelTexture::MipmapVoxelTexture() : _shader(nullptr)
{
}

MipmapVoxelTexture::~MipmapVoxelTexture()
{
	Destroy();

	SAFE_DELETE(_shader);
	SAFE_DELETE(_infoCB);
}

void MipmapVoxelTexture::Initialize(const GlobalInfo& giInfo)
{
	auto LoadComputeShader = [](const std::string& fileName, const std::string& mainFuncName) -> ComputeShader*
	{
		std::string filePath = "";
		EngineFactory pathFinder(nullptr);
		pathFinder.FetchShaderFullPath(filePath, fileName);

		ASSERT_COND_MSG(filePath.empty() == false, "Error, File path is empty!");

		ResourceManager* resourceManager = ResourceManager::SharedInstance();
		auto shaderMgr = resourceManager->GetShaderManager();

		ID3DBlob* blob = shaderMgr->CreateBlob(filePath, "cs", mainFuncName, false, nullptr);
		ComputeShader* shader = new ComputeShader(ComputeShader::ThreadGroup(0, 0, 0), blob);
		ASSERT_COND_MSG(shader->Initialize(), "can not create compute shader");

		return shader;
	};

#ifdef USE_ANISOTROPIC_VOXELIZATION
	_shader		= LoadComputeShader("MipmapAnisotropicVoxelTexture", "MipmapAnisotropicVoxelMapCS");
#else
	_shader		= LoadComputeShader("MipmapVoxelTexture", "MipmapVoxelMapCS");
#endif

	_infoCB = new ConstBuffer;
	_infoCB->Initialize(sizeof(InfoCB));
}

void MipmapVoxelTexture::Destroy()
{
}

void MipmapVoxelTexture::Mipmapping(const DirectX* dx, const VoxelMap* sourceColorMap, uint maxNumOfCascade)
{
	ID3D11DeviceContext* context	= dx->GetContext();
	uint sourceDimension			= sourceColorMap->GetSideLength();

	auto Mipmap = [&](ComputeShader* shader, const UnorderedAccessView* sourceUAV, const UnorderedAccessView* targetUAV, uint mipLevel, uint curCascade)
	{
		uint mipCoff		= 1 << mipLevel;
		uint curDimension	= sourceDimension / mipCoff;

		// Setting Thread Group
		{
			uint threadCount = (curDimension + MIPMAPPING_TILE_RES_HALF - 1) / MIPMAPPING_TILE_RES_HALF;
			shader->SetThreadGroupInfo(ComputeShader::ThreadGroup(threadCount, threadCount, threadCount));
		}

		InfoCB info;
		info.sourceDimension	= curDimension;
		info.currentCascade		= curCascade;
		_infoCB->UpdateSubResource(context, &info);

		std::vector<ShaderForm::InputUnorderedAccessView> uavs;
		{
			uavs.push_back(ShaderForm::InputUnorderedAccessView(uint(UAVBindIndex::VoxelMipmap_InputVoxelMap),	sourceUAV));
			uavs.push_back(ShaderForm::InputUnorderedAccessView(uint(UAVBindIndex::VoxelMipmap_OutputVoxelMap),	targetUAV));
		}
		shader->SetUAVs(uavs);

		std::vector<ShaderForm::InputConstBuffer> cbs;
		{
			cbs.push_back(ShaderForm::InputConstBuffer(uint(ConstBufferBindIndex::Mipmap_InfoCB), _infoCB));
		}
		shader->SetInputConstBuffers(cbs);

		shader->Dispatch(context);
	};

	if(sourceColorMap->GetMipmapCount() <= 1)
		return;

	for(uint curCascade=0; curCascade<maxNumOfCascade; ++curCascade)
	{
		Mipmap(_shader, sourceColorMap->GetSourceMapUAV(), sourceColorMap->GetMipmapUAV(0), 0, curCascade);

		uint maxMipLevel = sourceColorMap->GetMaxMipmapLevel();
		for(uint i=1; i<maxMipLevel; ++i)
			Mipmap(_shader, sourceColorMap->GetMipmapUAV(i-1), sourceColorMap->GetMipmapUAV(i), i, curCascade);
	}
}