#pragma once

#include "ComputeShader.h"
#include "ConstBuffer.h"
#include "VXGICommon.h"

#include "Voxelization.h"

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
		class VXGI
		{
		private:
			VXGIStaticInfo							_staticInfo;
			Buffer::ConstBuffer*					_staticInfoCB;

			VXGIDynamicInfo							_dynamicInfo;
			Buffer::ConstBuffer*					_dynamicInfoCB;

			VoxelMap*								_injectionColorMap;
			GPGPU::DirectCompute::ComputeShader*	_clearVoxelMapCS;

			Voxelization*							_voxelization;
			InjectRadianceFromPointLIght*			_injectPointLight;
			InjectRadianceFromSpotLIght*			_injectSpotLight;
			MipmapVoxelTexture*						_mipmap;
			VoxelConeTracing*						_voxelConeTracing;

			Debug::VoxelViewer*						_debugVoxelViewer;
			Math::Vector3							_startCenterWorldPos;

		public:
			VXGI();
			~VXGI();

		private:
			void InitializeClearVoxelMap(uint dimension);
			void ClearInjectColorVoxelMap(const Device::DirectX* dx);
			void UpdateGIDynamicInfo(const Device::DirectX* dx, const VXGIDynamicInfo& dynamicInfo);

		public:
			void Initialize(const Device::DirectX* dx, uint dimension = 256, float minWorldSize = 4.0f);
			void Run(const Device::DirectX* dx, const Camera::MeshCamera* camera, const Core::Scene* scene);
			void Destroy();

		public:
			GET_ACCESSOR(IndirectColorMap, const Texture::RenderTexture*,	_voxelConeTracing->GetIndirectColorMap());
			GET_ACCESSOR(DebugVoxelViewer, const Debug::VoxelViewer*,		_debugVoxelViewer);
			GET_SET_ACCESSOR(StartCenterWorldPos, const Math::Vector3&,		_startCenterWorldPos);
		};
	}
}