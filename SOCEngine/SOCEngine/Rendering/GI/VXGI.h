#pragma once

#include "ComputeShader.h"
#include "ConstBuffer.h"
#include "VXGICommon.h"

#include "Voxelization.h"

#include "InjectRadianceFromPointLight.h"
#include "InjectRadianceFromSpotLight.h"

#include "MipmapVoxelMap.h"

#include "VoxelConeTracing.h"
#include "ShadowRenderer.h"

namespace Rendering
{
	namespace GI
	{
		class VXGI
		{
		public:
			void Initialize(Device::DirectX& dx, Manager::ShaderManager& shaderMgr, uint dimension = 256, float minWorldSize = 4.0f);
			void Run(Device::DirectX& dx, MainRenderingSystemParam& main, Manager::LightManager& lightMgr, ShadowSystemParam& shadowSystem);

			void UpdateGIDynamicInfoCB(Device::DirectX& dx, uint packedNumfOfLights);
			GET_SET_ACCESSOR(StartCenterWorldPos, const Math::Vector3&, _startCenterWorldPos);

		private:
			void InitializeClearVoxelMap(Device::DirectX& dx, Manager::ShaderManager& shaderMgr, uint dimension);
			void ClearInjectColorVoxelMap(Device::DirectX& dx);

		private:
			VXGIStaticInfo							_staticInfo;
			VXGIInfoCB								_infoCB;

			VoxelMap								_injectionSourceMap;
			VoxelMap								_mipmappedInjectionMap;

			Shader::ComputeShader					_clearVoxelMap;
			Voxelization							_voxelization;
			InjectRadianceFromPointLight			_injectPointLight;
			InjectRadianceFromSpotLight				_injectSpotLight;
			MipmapVoxelMap							_mipmap;
			VoxelConeTracing						_voxelConeTracing;

			Math::Vector3							_startCenterWorldPos = Math::Vector3(0.0f, 0.0f, 0.0f);
		};
	}
}
