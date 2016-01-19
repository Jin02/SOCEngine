#pragma once

#include "ComputeShader.h"
#include "ConstBuffer.h"
#include "GlobalIlluminationCommon.h"

#include "ShadingWithLightCulling.h"
#include "AnisotropicVoxelMapAtlas.h"

namespace Rendering
{
	namespace GI
	{
		class VoxelConeTracing
		{
		public:
			struct DirectLightingParam
			{
				TBDR::ShadingWithLightCulling::GBuffers	gbuffer;
				const Texture::DepthBuffer*				opaqueDepthBuffer;
				const Texture::RenderTexture*			directLightingColorMap;
			};

		private:
			GPGPU::DirectCompute::ComputeShader*	_shader;
			Texture::RenderTexture*					_indirectColorMap;

		public:
			VoxelConeTracing();
			~VoxelConeTracing();

		public:
			void Initialize(const Device::DirectX* dx);
			void Run(const Device::DirectX* dx, const AnisotropicVoxelMapAtlas* mipmappedVoxelColorMap, const DirectLightingParam& param);
			void Destroy();

		public:
			GET_ACCESSOR(IndirectColorMap, const Texture::RenderTexture*, _indirectColorMap);
		};
	}
}