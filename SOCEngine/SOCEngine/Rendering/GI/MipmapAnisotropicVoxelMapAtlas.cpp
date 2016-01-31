#include "MipmapAnisotropicVoxelMapAtlas.h"
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

MipmapAnisotropicVoxelMapAtlas::MipmapAnisotropicVoxelMapAtlas()
	: _blockToAnisotropicMipmapShader(nullptr), _anisotropicMipmapShader(nullptr), _infoCB(nullptr), _anisotropicColorMap(nullptr)
{
}

MipmapAnisotropicVoxelMapAtlas::~MipmapAnisotropicVoxelMapAtlas()
{
	Destroy();

	SAFE_DELETE(_blockToAnisotropicMipmapShader);
	SAFE_DELETE(_anisotropicMipmapShader);

	SAFE_DELETE(_anisotropicColorMap);

	SAFE_DELETE(_infoCB);
}

void MipmapAnisotropicVoxelMapAtlas::Initialize(const GlobalInfo& giInfo)
{
	auto LoadComputeShader = [](const std::string& fileName, const std::string& mainFuncName, bool useAnisotropicMipmap) -> ComputeShader*
	{
		std::string filePath = "";
		EngineFactory pathFinder(nullptr);
		pathFinder.FetchShaderFullPath(filePath, fileName);

		ASSERT_COND_MSG(filePath.empty() == false, "Error, File path is empty!");

		ResourceManager* resourceManager = ResourceManager::SharedInstance();
		auto shaderMgr = resourceManager->GetShaderManager();

		std::vector<ShaderMacro> macros;
		if(useAnisotropicMipmap)
		{
			macros.push_back(ShaderMacro("ANISOTROPIC_MIPMAP", ""));
		}

		ID3DBlob* blob = shaderMgr->CreateBlob(filePath, "cs", mainFuncName, false, &macros);
		ComputeShader* shader = new ComputeShader(ComputeShader::ThreadGroup(0, 0, 0), blob);
		ASSERT_COND_MSG(shader->Initialize(), "can not create compute shader");

		return shader;
	};

	_blockToAnisotropicMipmapShader	= LoadComputeShader("MipmapAnisotropicVoxelTexture", "MipmapAnisotropicVoxelMapCS", false);
	_anisotropicMipmapShader		= LoadComputeShader("MipmapAnisotropicVoxelTexture", "MipmapAnisotropicVoxelMapCS", true);

	_infoCB = new ConstBuffer;
	_infoCB->Initialize(sizeof(InfoCB));


#ifndef USE_ANISOTROPIC_VOXELIZATION
	_anisotropicColorMap = new VoxelMap;
	uint dimension = 1 << (giInfo.voxelDimensionPow2 - 1);
	_anisotropicColorMap->Initialize(dimension, giInfo.maxNumOfCascade, DXGI_FORMAT_R8G8B8A8_TYPELESS, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM, giInfo.maxMipLevel - 1, true);
#endif
}

void MipmapAnisotropicVoxelMapAtlas::Destroy()
{
#ifndef USE_ANISOTROPIC_VOXELIZATION
	_anisotropicColorMap->Destory();
#endif
}

void MipmapAnisotropicVoxelMapAtlas::Mipmapping(const DirectX* dx, const VoxelMap* sourceColorMap, uint maxNumOfCascade)
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

#ifdef USE_ANISOTROPIC_VOXELIZATION
	if(sourceColorMap->GetMipmapCount() <= 1)
		return;

	for(uint curCascade=0; curCascade<maxNumOfCascade; ++curCascade)
	{
		Mipmap(_anisotropicMipmapShader, sourceColorMap->GetSourceMapUAV(), sourceColorMap->GetMipmapUAV(0), 0, curCascade);

		uint maxMipLevel = sourceColorMap->GetMaxMipmapLevel();
		for(uint i=1; i<maxMipLevel; ++i)
			Mipmap(_anisotropicMipmapShader,sourceColorMap->GetMipmapUAV(i-1), sourceColorMap->GetMipmapUAV(i), i, curCascade);
	}
#else
	for(uint curCascade = 0; curCascade < maxNumOfCascade; ++curCascade)
	{
		Mipmap(_blockToAnisotropicMipmapShader, sourceColorMap->GetSourceMapUAV(), _anisotropicColorMap->GetSourceMapUAV(), 0, curCascade);
		Mipmap(_anisotropicMipmapShader, _anisotropicColorMap->GetSourceMapUAV(), _anisotropicColorMap->GetMipmapUAV(0), 1, curCascade);

		uint maxMipLevel = _anisotropicColorMap->GetMaxMipmapLevel();
		for(uint i=1; i<maxMipLevel; ++i)
			Mipmap(_anisotropicMipmapShader, _anisotropicColorMap->GetMipmapUAV(i-1), _anisotropicColorMap->GetMipmapUAV(i), (i+1), curCascade);
	}

#endif
}