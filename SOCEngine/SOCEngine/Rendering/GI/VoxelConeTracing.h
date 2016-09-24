#pragma once

#include "ComputeShader.h"
#include "ConstBuffer.h"
#include "VXGICommon.h"

#include "ShadingWithLightCulling.h"
#include "VoxelMap.h"

#include "MeshCamera.h"

#include "BilateralFiltering.h"
#include "GaussianBlur.h"

#define USE_BILATERAL_FILTERING

namespace Rendering
{
	namespace GI
	{
		class VoxelConeTracing
		{
		private:
			GPGPU::DirectCompute::ComputeShader*	_shader;
			Texture::RenderTexture*					_indirectColorMap;

#if defined(USE_GAUSSIAN_BLUR)
			PostProcessing::GaussianBlur*			_blur;
#elif defined(USE_BILATERAL_FILTERING)
			PostProcessing::BilateralFiltering*		_blur;
#endif

		public:
			VoxelConeTracing();
			~VoxelConeTracing();

		public:
			void Initialize(const Device::DirectX* dx);
			void Run(const Device::DirectX* dx, const VoxelMap* injectedColorMap, const Camera::MeshCamera* meshCam, const Buffer::ConstBuffer* vxgiStaticInfoCB, const Buffer::ConstBuffer* vxgiDynamicInfoCB);
			void Destroy();

		public:
			GET_ACCESSOR(IndirectColorMap, const Texture::RenderTexture*, _indirectColorMap);
		};
	}
}