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
	: _shader(nullptr), _infoCB(nullptr)
{
}

MipmapAnisotropicVoxelMapAtlas::~MipmapAnisotropicVoxelMapAtlas()
{
	Destroy();

	SAFE_DELETE(_shader);
	SAFE_DELETE(_infoCB);
}

void MipmapAnisotropicVoxelMapAtlas::Initialize(bool useAnisotropicMipmap)
{
	std::string filePath = "";
	EngineFactory pathFinder(nullptr);
	pathFinder.FetchShaderFullPath(filePath, "MipmapAnisotropicVoxelTexture");

	ASSERT_COND_MSG(filePath.empty() == false, "Error, File path is empty!");

	ResourceManager* resourceManager = ResourceManager::SharedInstance();
	auto shaderMgr = resourceManager->GetShaderManager();

	std::vector<ShaderMacro> macros;
	if(useAnisotropicMipmap)
	{
		macros.push_back(ShaderMacro("ANISOTROPIC_MIPMAP", ""));
	}

	ID3DBlob* blob = shaderMgr->CreateBlob(filePath, "cs", "MipmapAnisotropicVoxelMapCS", false, &macros);
	_shader = new ComputeShader(ComputeShader::ThreadGroup(0, 0, 0), blob);
	ASSERT_COND_MSG(_shader->Initialize(), "can not create compute shader");

	_infoCB = new ConstBuffer;
	_infoCB->Initialize(sizeof(InfoCB));
}

void MipmapAnisotropicVoxelMapAtlas::Destroy()
{

}

void MipmapAnisotropicVoxelMapAtlas::Mipmapping(const DirectX* dx, const AnisotropicVoxelMapAtlas* colorMap, uint maxNumOfCascade)
{
	if(colorMap->GetMipmapCount() <= 1)
		return;

	ID3D11DeviceContext* context				= dx->GetContext();
	uint dimension								= colorMap->GetSideLength();

	auto Mipmap = [&](const UnorderedAccessView* sourceUAV, const UnorderedAccessView* targetUAV, uint mipLevel, uint curCascade)
	{
		uint mipCoff = 1 << mipLevel;

		// Setting Thread Group
		{
			uint threadCount = (dimension / mipCoff + MIPMAPPING_TILE_RES_HALF - 1) / MIPMAPPING_TILE_RES_HALF;
			_shader->SetThreadGroupInfo(ComputeShader::ThreadGroup(threadCount, threadCount, threadCount));
		}

		InfoCB info;
		info.sourceDimension	= dimension / mipCoff;
		info.currentCascade		= curCascade;
		_infoCB->UpdateSubResource(context, &info);

		std::vector<ShaderForm::InputUnorderedAccessView> uavs;
		{
			uavs.push_back(ShaderForm::InputUnorderedAccessView(uint(UAVBindIndex::VoxelMipmap_InputVoxelMap),	sourceUAV));
			uavs.push_back(ShaderForm::InputUnorderedAccessView(uint(UAVBindIndex::VoxelMipmap_OutputVoxelMap),	targetUAV));
		}
		_shader->SetUAVs(uavs);

		std::vector<ShaderForm::InputConstBuffer> cbs;
		{
			cbs.push_back(ShaderForm::InputConstBuffer(uint(ConstBufferBindIndex::Mipmap_InfoCB), _infoCB));
		}
		_shader->SetInputConstBuffers(cbs);

		_shader->Dispatch(context);
	};

	for(uint curCascade=0; curCascade<maxNumOfCascade; ++curCascade)
	{
		Mipmap(colorMap->GetSourceMapUAV(), colorMap->GetMipmapUAV(0), 0, curCascade);

		uint maxMipLevel = colorMap->GetMaxMipmapLevel();
		for(uint i=1; i<maxMipLevel; ++i)
			Mipmap(colorMap->GetMipmapUAV(i-1), colorMap->GetMipmapUAV(i), i, curCascade);
	}
}