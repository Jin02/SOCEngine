#pragma once

#include "ComputeShader.h"
#include "ConstBuffer.h"
#include "GlobalIlluminationCommon.h"

#include "Voxelization.h"

#include "InjectRadianceFromDirectionalLIght.h"
#include "InjectRadianceFromPointLIght.h"
#include "InjectRadianceFromSpotLIght.h"

#include "MipmapAnisotropicVoxelMapAtlas.h"

#include "VoxelConeTracing.h"
#include "RenderManager.h"
#include "ShadowRenderer.h"

namespace Rendering
{
	namespace GI
	{
		class GlobalIllumination
		{
		private:
			GlobalInfo								_globalInfo;
			Buffer::ConstBuffer*					_giGlobalInfoCB;
			AnisotropicVoxelMapAtlas*				_injectionColorMap;

			Voxelization*							_voxelization;

			InjectRadianceFromDirectionalLIght*		_injectDirectionalLight;
			InjectRadianceFromPointLIght*			_injectPointLight;
			InjectRadianceFromSpotLIght*			_injectSpotLight;

			MipmapAnisotropicVoxelMapAtlas*			_mipmap;
			VoxelConeTracing*						_voxelConeTracing;
			GPGPU::DirectCompute::ComputeShader*	_clearVoxelMapCS;

		public:
			GlobalIllumination();
			~GlobalIllumination();

		private:
			void InitializeClearVoxelMap(uint dimension, uint maxNumOfCascade);
			void ClearInjectColorVoxelMap(const Device::DirectX* dx);

		public:
			void Initialize(const Device::DirectX* dx, uint dimension = 256, float minWorldSize = 4.0f);
			void Run(const Device::DirectX* dx, const Camera::MeshCamera* camera, const Manager::RenderManager* renderManager, const Shadow::ShadowRenderer* shadowRenderer);
			void Destroy();

		public:
			GET_ACCESSOR(IndirectColorMap, const Texture::RenderTexture*, _voxelConeTracing->GetIndirectColorMap());
		};
	}
}