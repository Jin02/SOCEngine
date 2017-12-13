#include "VXGI.h"
#include "BindIndexInfo.h"
#include "ShaderFactory.hpp"
#include "MainRenderer.h"
#include "AutoBinder.hpp"

using namespace Device;
using namespace Core;
using namespace Rendering;
using namespace Rendering::Light;
using namespace Rendering::Shadow;
using namespace Rendering::Manager;
using namespace Rendering::Buffer;
using namespace Rendering::View;
using namespace Rendering::GI;
using namespace Rendering::Shader;
using namespace Rendering::Factory;
using namespace Rendering::Renderer;

void VXGI::InitializeClearVoxelMap(DirectX& dx, ShaderManager& shaderMgr, uint dimension)
{
	ShaderFactory factory(&shaderMgr);
	_clearVoxelMap = *factory.LoadComputeShader(dx, "ClearVoxelMap", "CS", nullptr, "@ClearVoxelMap");
}

void VXGI::ClearInjectColorVoxelMap(DirectX& dx)
{
	auto Clear = [&dx, &shader = _clearVoxelMap](UnorderedAccessView& uav, uint sideLength, bool isAnisotropic)
	{
		auto ComputeThreadGroupSideLength = [](uint sideLength)
		{
			return static_cast<uint>( static_cast<float>(sideLength + 8 - 1) / 8.0f );
		};
		
		AutoBinderUAV outUAV(dx, UAVBindIndex(0), uav);
		uint yz = ComputeThreadGroupSideLength(sideLength);
		shader.Dispatch(dx, ComputeShader::ThreadGroup(ComputeThreadGroupSideLength(sideLength * (isAnisotropic ? (uint)VoxelMap::Direction::Num : 1)), yz, yz));
	};

	Clear(_injectionSourceMap.GetSourceMapUAV(), _staticInfo.dimension, false);
	Clear(_mipmappedInjectionMap.GetSourceMapUAV(), _staticInfo.dimension / 2, true);
}

void VXGI::Initialize(DirectX& dx, ShaderManager& shaderMgr, const Size<uint>& renderSize, const VXGIStaticInfo&& info)
{
	// Setting Infos
	{
		_staticInfo = info;
		_infoCB.staticInfoCB.Initialize(dx);
		_infoCB.staticInfoCB.UpdateSubResource(dx, info);

		_infoCB.dynamicInfoCB.Initialize(dx);
		UpdateGIDynamicInfoCB(dx);
	}

	// Init Voxelization
	_voxelization.Initialize(dx, shaderMgr, _staticInfo.dimension, _staticInfo.voxelSize);

	// Injection
	{
		_injectionSourceMap.Initialize(dx, _staticInfo.dimension, DXGI_FORMAT_R8G8B8A8_TYPELESS, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R32_UINT, 1, false);
		_mipmappedInjectionMap.Initialize(dx, _staticInfo.dimension / 2, DXGI_FORMAT_R8G8B8A8_TYPELESS, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R32_UINT, uint(_staticInfo.maxMipLevel) - 1, true);

		_injectPointLight.Initialize(dx, shaderMgr, _staticInfo.dimension);
		_injectSpotLight.Initialize(dx, shaderMgr, _staticInfo.dimension);
	}

	// Mipmap
	_mipmap.Initialize(dx, shaderMgr);

	// Voxel Cone Tracing
	_voxelConeTracing.Initialize(dx, shaderMgr, renderSize);

	InitializeClearVoxelMap(dx, shaderMgr, _staticInfo.dimension);

	_indirectColorMap.Initialize(dx, renderSize, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, 0, 1);
}

void VXGI::Run(DirectX& dx, const VXGI::Param&& param)
{	
	ClearInjectColorVoxelMap(dx);
	
	const auto& lightMgr		= param.lightMgr;
	const auto& tbrCB			= param.main.renderer.GetTBRParamCB();

	// 1. Voxelization Pass
	{
		if (_dirtyVoxelizeCenterPos)
			_voxelization.UpdateConstBuffer(dx, _dynamicInfo.startCenterWorldPos);

		// Clear Voxel Map and voxelize
		_voxelization.Voxelize(dx, _injectionSourceMap,
			Voxelization::Param{_infoCB, lightMgr, param.shadowParam, tbrCB,
			param.cullParam, param.meshRenderParam, param.materialMgr}
		);
	}

	// 2. Injection Pass
	{				
		InjectRadianceFormUtility::BindParam injParam{
			_infoCB, _voxelization, tbrCB, param.shadowParam.manager.GetGlobalCB()
		};

		if(lightMgr.GetLightCount<PointLight>() > 0)
			_injectPointLight.Inject(dx, _injectionSourceMap, lightMgr, param.shadowParam, injParam);

		if(lightMgr.GetLightCount<SpotLight>() > 0)
			_injectSpotLight.Inject(dx, _injectionSourceMap, lightMgr, param.shadowParam, injParam);
	}

	// 3. Mipmap Pass
	_mipmap.Mipmapping(dx, _injectionSourceMap, _mipmappedInjectionMap);

	// 4. Voxel Cone Tracing Pass
	_indirectColorMap.Clear(dx, Color::Clear());
	_voxelConeTracing.Run(dx, _indirectColorMap, {_injectionSourceMap, _mipmappedInjectionMap, _infoCB, param.main});

	_dirtyVoxelizeCenterPos = false;
}

void VXGI::UpdateGIDynamicInfoCB(DirectX& dx)
{
	_infoCB.dynamicInfoCB.UpdateSubResource(dx, _dynamicInfo);
	_dirty = false;
}