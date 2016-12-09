#include "MipmapVoxelMap.h"
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

MipmapVoxelMap::MipmapVoxelMap() : _baseMipmap(nullptr), _anisotropicMipmap(nullptr)
{
}

MipmapVoxelMap::~MipmapVoxelMap()
{
	Destroy();

	SAFE_DELETE(_baseMipmap);
	SAFE_DELETE(_anisotropicMipmap);

	SAFE_DELETE(_infoCB);
}

void MipmapVoxelMap::Initialize()
{
	auto LoadComputeShader = [](const std::string& fileName, const std::string& mainFuncName, bool isInputAnisotropicMap) -> ComputeShader*
	{
		std::string filePath = "";
		EngineFactory pathFinder(nullptr);
		pathFinder.FetchShaderFullPath(filePath, fileName);

		ASSERT_MSG_IF(filePath.empty() == false, "Error, File path is empty!");

		ResourceManager* resourceManager = ResourceManager::SharedInstance();
		auto shaderMgr = resourceManager->GetShaderManager();

		std::vector<ShaderMacro> macros;
		if(isInputAnisotropicMap == false)
			macros.push_back(ShaderMacro("BASE_MIP_MAP", ""));

		ID3DBlob* blob = shaderMgr->CreateBlob(filePath, "cs", mainFuncName, false, &macros);
		ComputeShader* shader = new ComputeShader(ComputeShader::ThreadGroup(0, 0, 0), blob);
		ASSERT_MSG_IF(shader->Initialize(), "can not create compute shader");

		return shader;
	};

	_baseMipmap			= LoadComputeShader("MipmapAnisotropicVoxelTexture", "MipmapAnisotropicVoxelMapCS", false);
	_anisotropicMipmap	= LoadComputeShader("MipmapAnisotropicVoxelTexture", "MipmapAnisotropicVoxelMapCS", true);

	_infoCB = new ConstBuffer;
	_infoCB->Initialize(sizeof(InfoCB));
}

void MipmapVoxelMap::Destroy()
{
	_infoCB->Destroy();
}

void MipmapVoxelMap::Mipmapping(const DirectX* dx, const VoxelMap* sourceColorMap, const VoxelMap* anisotropicMap)
{
	ID3D11DeviceContext* context	= dx->GetContext();
	uint sourceDimension			= sourceColorMap->GetSideLength();

	auto Mipmap = [&](ComputeShader* shader, const UnorderedAccessView* sourceUAV, const UnorderedAccessView* targetUAV, uint mipLevel)
	{
		uint mipCoff		= 1 << mipLevel;
		uint curDimension	= sourceDimension / mipCoff;

		// Setting Thread Group
		{
			uint threadCount = ((curDimension/2) + MIPMAPPING_TILE_RES_HALF - 1) / MIPMAPPING_TILE_RES_HALF;
			shader->SetThreadGroupInfo(ComputeShader::ThreadGroup(threadCount, threadCount, threadCount));
		}

		InfoCB info;
		info.sourceDimension	= curDimension;
		_infoCB->UpdateSubResource(context, &info);

		ComputeShader::BindUnorderedAccessView(context, UAVBindIndex::VoxelMipmap_InputVoxelMap,	sourceUAV);
		ComputeShader::BindUnorderedAccessView(context, UAVBindIndex::VoxelMipmap_OutputVoxelMap,	targetUAV);
		ComputeShader::BindConstBuffer(context, ConstBufferBindIndex::Mipmap_InfoCB,				_infoCB);

		shader->Dispatch(context);

		ComputeShader::BindUnorderedAccessView(context, UAVBindIndex::VoxelMipmap_InputVoxelMap,	nullptr);
		ComputeShader::BindUnorderedAccessView(context, UAVBindIndex::VoxelMipmap_OutputVoxelMap,	nullptr);
		ComputeShader::BindConstBuffer(context, ConstBufferBindIndex::Mipmap_InfoCB,				nullptr);
	};

	ASSERT_MSG_IF(sourceColorMap && anisotropicMap, "Error, param is invalid");

	Mipmap(_baseMipmap, sourceColorMap->GetSourceMapUAV(), anisotropicMap->GetSourceMapUAV(), 0);
	Mipmap(_anisotropicMipmap, anisotropicMap->GetSourceMapUAV(), anisotropicMap->GetMipmapUAV(0), 1);

	uint maxMipLevel = anisotropicMap->GetMaxMipmapLevel();
	for(uint i=1; i<maxMipLevel; ++i)
		Mipmap(_anisotropicMipmap, anisotropicMap->GetMipmapUAV(i-1), anisotropicMap->GetMipmapUAV(i), i+1);
}