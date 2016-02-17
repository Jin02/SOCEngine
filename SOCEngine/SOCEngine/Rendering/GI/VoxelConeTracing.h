#pragma once

#include "ComputeShader.h"
#include "ConstBuffer.h"
#include "GlobalIlluminationCommon.h"

#include "ShadingWithLightCulling.h"
#include "VoxelMap.h"

#include "MeshCamera.h"

namespace Rendering
{
	namespace GI
	{
		class VoxelConeTracing
		{
		private:
			GPGPU::DirectCompute::ComputeShader*	_shader;
			Texture::RenderTexture*					_indirectColorMap;

		public:
			VoxelConeTracing();
			~VoxelConeTracing();

		public:
			void Initialize(const Device::DirectX* dx, const Buffer::ConstBuffer* giInfoCB);
			void Run(const Device::DirectX* dx, const VoxelMap* injectedColorMap, const Camera::MeshCamera* meshCam);
			void Destroy();

		public:
			GET_ACCESSOR(IndirectColorMap, const Texture::RenderTexture*, _indirectColorMap);
		};
	}
}