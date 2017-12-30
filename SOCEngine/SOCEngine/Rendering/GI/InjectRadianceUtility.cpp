#include "InjectRadianceUtility.h"

using namespace Device;
using namespace Core;
using namespace Rendering;
using namespace Rendering::Shadow;
using namespace Rendering::Manager;
using namespace Rendering::Buffer;
using namespace Rendering::GI;
using namespace Rendering::Shader;
using namespace Rendering::RenderState;

void InjectRadianceFormUtility::Bind(DirectX& dx, VoxelMap& outVoxelMap, const BindParam& param)
{
	ComputeShader::BindSamplerState(dx, SamplerStateBindIndex::ShadowComprisonGreaterSamplerState,	SamplerState::ShadowGreaterEqualComp);
	ComputeShader::BindSamplerState(dx, SamplerStateBindIndex::ShadowPointSamplerState,				SamplerState::Point);

	ComputeShader::BindConstBuffer(dx, ConstBufferBindIndex::VXGIStaticInfoCB,				param.infoCB.staticInfoCB);
	ComputeShader::BindConstBuffer(dx, ConstBufferBindIndex::VXGIDynamicInfoCB,				param.infoCB.dynamicInfoCB);
	ComputeShader::BindConstBuffer(dx, ConstBufferBindIndex::ShadowGlobalParam,				param.shadowGlobalParamCB);
	ComputeShader::BindConstBuffer(dx, ConstBufferBindIndex::VoxelizationInfoCB,			param.voxelization.GetInfoCB());
	ComputeShader::BindConstBuffer(dx, ConstBufferBindIndex::TBRParam,						param.tbrParamCB);

	ComputeShader::BindUnorderedAccessView(dx, UAVBindIndex(0),								*outVoxelMap.GetSourceMapUAV());
	ComputeShader::BindUnorderedAccessView(dx, UAVBindIndex(1),								*param.voxelization.GetVoxelAlbedoRawBuffer().GetUnorderedAccessView());
	ComputeShader::BindUnorderedAccessView(dx, UAVBindIndex(2),								*param.voxelization.GetVoxelNormalRawBuffer().GetUnorderedAccessView());
	ComputeShader::BindUnorderedAccessView(dx, UAVBindIndex(3),								*param.voxelization.GetVoxelEmissionRawBuffer().GetUnorderedAccessView());
}

void InjectRadianceFormUtility::UnBind(Device::DirectX & dx)
{
	ComputeShader::UnBindSamplerState(dx, SamplerStateBindIndex::ShadowComprisonGreaterSamplerState);
	ComputeShader::UnBindSamplerState(dx, SamplerStateBindIndex::ShadowPointSamplerState);

	ComputeShader::UnBindConstBuffer(dx, ConstBufferBindIndex::VXGIStaticInfoCB);
	ComputeShader::UnBindConstBuffer(dx, ConstBufferBindIndex::VXGIDynamicInfoCB);
	ComputeShader::UnBindConstBuffer(dx, ConstBufferBindIndex::ShadowGlobalParam);
	ComputeShader::UnBindConstBuffer(dx, ConstBufferBindIndex::VoxelizationInfoCB);
	ComputeShader::UnBindConstBuffer(dx, ConstBufferBindIndex::TBRParam);

	ComputeShader::UnBindUnorderedAccessView(dx, UAVBindIndex::OutVoxelColorMap);

	ComputeShader::UnBindUnorderedAccessView(dx, UAVBindIndex(1));
	ComputeShader::UnBindUnorderedAccessView(dx, UAVBindIndex(2));
	ComputeShader::UnBindUnorderedAccessView(dx, UAVBindIndex(3));
}

uint InjectRadianceFormUtility::CalcThreadSideLength(uint dimension)
{
	return (dimension + INJECTION_TILE_RES - 1) / INJECTION_TILE_RES;
}
