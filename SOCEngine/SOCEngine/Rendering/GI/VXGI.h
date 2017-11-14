#pragma once

#include "ComputeShader.h"
#include "ConstBuffer.h"
#include "VXGICommon.h"

#include "Voxelization.h"

#include "InjectRadianceFromPointLight.h"
#include "InjectRadianceFromSpotLight.h"

#include "MipmapVoxelMap.h"

#include "VoxelConeTracing.h"
#include "ShadowAtlasMapRenderer.h"

namespace Rendering
{
	namespace GI
	{
		class VXGI final
		{
		public:
			struct Param
			{
				const MainRenderingSystemParam&&		main;
				const Manager::LightManager&			lightMgr;
				const ShadowSystemParam&				shadowSystem;
				const Renderer::CullingParam&			cullParam;
				const Renderer::MeshRenderer::Param&	meshRenderParam;
				const Manager::MaterialManager&			materialMgr;
			};

			GET_CONST_ACCESSOR(StartCenterWorldPos, const Math::Vector3&,	_dynamicInfo.startCenterWorldPos);
			SET_ACCESSOR_DIRTY(StartCenterWorldPos,	const Math::Vector3&,	_dynamicInfo.startCenterWorldPos);
			SET_ACCESSOR_DIRTY(PackedNumfOfLights,	uint,					_dynamicInfo.packedNumfOfLights);

			GET_CONST_ACCESSOR(StaticInfo,			const VXGIStaticInfo&,	_staticInfo);
			GET_CONST_ACCESSOR(Dirty,				bool,					_dirty);

			GET_ACCESSOR(IndirectColorMap,			auto&,					_indirectColorMap);


		public:
			void Initialize(Device::DirectX& dx, Manager::ShaderManager& shaderMgr, const Size<uint>& renderSize, const VXGIStaticInfo&& info);
			void Run(Device::DirectX& dx, const Param&& param);
			void UpdateGIDynamicInfoCB(Device::DirectX& dx);
			
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
			Texture::RenderTexture					_indirectColorMap;

			VXGIDynamicInfo							_dynamicInfo;
			bool									_dirty = true;
		};
	}
}
