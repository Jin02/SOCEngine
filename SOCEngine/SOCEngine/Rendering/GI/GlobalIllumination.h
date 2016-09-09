#pragma once

#include "ComputeShader.h"
#include "ConstBuffer.h"
#include "GlobalIlluminationCommon.h"

#include "Voxelization.h"

#include "InjectRadianceFromDirectionalLIght.h"
#include "InjectRadianceFromPointLIght.h"
#include "InjectRadianceFromSpotLIght.h"

#include "MipmapVoxelTexture.h"

#include "VoxelConeTracing.h"
#include "RenderManager.h"
#include "ShadowRenderer.h"

#include "VoxelViewer.h"

namespace Core
{
	class Scene;
}

namespace Rendering
{
	namespace GI
	{
		class GlobalIllumination
		{
		private:
			GlboalStaticInfo						_globalStaticInfo;
			Buffer::ConstBuffer*					_giGlobalStaticInfoCB;
			GlobalDynamicInfo						_globalDynamicInfo;
			Buffer::ConstBuffer*					_giGlobalDynamicInfoCB;


			VoxelMap*								_injectionColorMap;

			Voxelization*							_voxelization;

			InjectRadianceFromPointLIght*			_injectPointLight;
			InjectRadianceFromSpotLIght*			_injectSpotLight;

			MipmapVoxelTexture*						_mipmap;

			VoxelConeTracing*						_voxelConeTracing;
			GPGPU::DirectCompute::ComputeShader*	_clearVoxelMapCS;

			bool									_isAttachCamera;

			Debug::VoxelViewer*						_debugVoxelViewer;

		public:
			GlobalIllumination();
			~GlobalIllumination();

		private:
			void InitializeClearAnisotropicVoxelMap(uint dimension, uint maxNumOfCascade);
			void ClearInjectColorVoxelMap(const Device::DirectX* dx);

		public:
			void Initialize(const Device::DirectX* dx, uint dimension = 256, float minWorldSize = 4.0f);
			void UpdateGIDynamicInfo(const Device::DirectX* dx, const GlobalDynamicInfo& dynamicInfo);
			void Run(const Device::DirectX* dx, const Camera::MeshCamera* camera, const Core::Scene* scene);
			void Destroy();

		public:
			GET_ACCESSOR(IndirectColorMap,		const Texture::RenderTexture*,	_voxelConeTracing->GetIndirectColorMap());
			GET_ACCESSOR(DebugVoxelViewer,		const Debug::VoxelViewer*,		_debugVoxelViewer);
			GET_SET_ACCESSOR(IsAttachCamera,	bool,							_isAttachCamera);
		};
	}
}