#include "MipmapVoxelMap.h"
#include "ShaderFactory.hpp"

using namespace Device;
using namespace Core;
using namespace Rendering;
using namespace Rendering::Factory;
using namespace Rendering::Manager;
using namespace Rendering::Buffer;
using namespace Rendering::GI;
using namespace Rendering::View;
using namespace Rendering::Shader;
using namespace Rendering::Factory;

void MipmapVoxelMap::Initialize(DirectX& dx, ShaderManager& shaderMgr)
{
	std::vector<ShaderMacro> baseMipmapMacro{ ShaderMacro("BASE_MIP_MAP") };

	ShaderFactory factory(&shaderMgr);
	_baseMipmap			= *factory.LoadComputeShader(dx, "MipmapAnisotropicVoxelTexture", "MipmapAnisotropicVoxelMapCS", &baseMipmapMacro, "@MipmapVoxelBase");
	_anisotropicMipmap	= *factory.LoadComputeShader(dx, "MipmapAnisotropicVoxelTexture", "MipmapAnisotropicVoxelMapCS", nullptr, "@MipmapVoxelAnisotropic");

	_infoCB.Initialize(dx);
}

void MipmapVoxelMap::Mipmapping(DirectX& dx, const VoxelMap& sourceColorMap, VoxelMap& outAnisotropicMap)
{
	uint sourceDimension = sourceColorMap.GetSideLength();

	auto Mipmap = [&dx, sourceDimension, &infoCB = _infoCB](ComputeShader& shader, const UnorderedAccessView& sourceUAV, UnorderedAccessView& targetUAV, uint mipLevel)
	{
		uint mipCoff		= 1 << mipLevel;
		uint curDimension	= sourceDimension / mipCoff;

		InfoCBData info;
		info.sourceDimension = curDimension;
		infoCB.UpdateSubResource(dx, info);

		ComputeShader::BindUnorderedAccessView(dx,	UAVBindIndex::VoxelMipmap_InputVoxelMap,	sourceUAV);
		ComputeShader::BindUnorderedAccessView(dx,	UAVBindIndex::VoxelMipmap_OutputVoxelMap,	targetUAV);
		ComputeShader::BindConstBuffer(dx,			ConstBufferBindIndex::Mipmap_InfoCB,		infoCB);

		uint threadCount = ((curDimension/2) + MIPMAPPING_TILE_RES_HALF - 1) / MIPMAPPING_TILE_RES_HALF;
		shader.Dispatch(dx, ComputeShader::ThreadGroup(threadCount, threadCount, threadCount));

		ComputeShader::UnBindUnorderedAccessView(dx,	UAVBindIndex::VoxelMipmap_InputVoxelMap);
		ComputeShader::UnBindUnorderedAccessView(dx,	UAVBindIndex::VoxelMipmap_OutputVoxelMap);
		ComputeShader::UnBindConstBuffer(dx,			ConstBufferBindIndex::Mipmap_InfoCB);
	};

	Mipmap(_baseMipmap, sourceColorMap.GetSourceMapUAV(), outAnisotropicMap.GetSourceMapUAV(), 0);
	Mipmap(_anisotropicMipmap, outAnisotropicMap.GetSourceMapUAV(), outAnisotropicMap.GetMipmapUAV(0), 1);

	uint maxMipLevel = outAnisotropicMap.GetMaxMipmapLevel();
	for(uint i=1; i<maxMipLevel; ++i)
		Mipmap(_anisotropicMipmap, outAnisotropicMap.GetMipmapUAV(i-1), outAnisotropicMap.GetMipmapUAV(i), i+1);
}