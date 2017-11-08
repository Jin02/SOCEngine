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

void MipmapVoxelMap::Mipmapping(DirectX& dx, VoxelMap& sourceColorMap, VoxelMap& anisotropicMap)
{
	uint sourceDimension = sourceColorMap.GetSideLength();

	auto Mipmap = [&dx, sourceDimension, &infoCB = _infoCB](ComputeShader& shader, UnorderedAccessView& sourceUAV, UnorderedAccessView& targetUAV, uint mipLevel)
	{
		uint mipCoff		= 1 << mipLevel;
		uint curDimension	= sourceDimension / mipCoff;

		// Setting Thread Group
		{
			uint threadCount = ((curDimension/2) + MIPMAPPING_TILE_RES_HALF - 1) / MIPMAPPING_TILE_RES_HALF;
			shader.SetThreadGroupInfo(ComputeShader::ThreadGroup(threadCount, threadCount, threadCount));
		}

		InfoCBData info;
		info.sourceDimension = curDimension;
		infoCB.UpdateSubResource(dx, info);

		ComputeShader::BindUnorderedAccessView(dx,	UAVBindIndex::VoxelMipmap_InputVoxelMap,	sourceUAV);
		ComputeShader::BindUnorderedAccessView(dx,	UAVBindIndex::VoxelMipmap_OutputVoxelMap,	targetUAV);
		ComputeShader::BindConstBuffer(dx,			ConstBufferBindIndex::Mipmap_InfoCB,		infoCB);

		shader.Dispatch(dx);

		ComputeShader::UnBindUnorderedAccessView(dx,	UAVBindIndex::VoxelMipmap_InputVoxelMap);
		ComputeShader::UnBindUnorderedAccessView(dx,	UAVBindIndex::VoxelMipmap_OutputVoxelMap);
		ComputeShader::UnBindConstBuffer(dx,			ConstBufferBindIndex::Mipmap_InfoCB);
	};

	Mipmap(_baseMipmap, sourceColorMap.GetSourceMapUAV(), anisotropicMap.GetSourceMapUAV(), 0);
	Mipmap(_anisotropicMipmap, anisotropicMap.GetSourceMapUAV(), anisotropicMap.GetMipmapUAV(0), 1);

	uint maxMipLevel = anisotropicMap.GetMaxMipmapLevel();
	for(uint i=1; i<maxMipLevel; ++i)
		Mipmap(_anisotropicMipmap, anisotropicMap.GetMipmapUAV(i-1), anisotropicMap.GetMipmapUAV(i), i+1);
}