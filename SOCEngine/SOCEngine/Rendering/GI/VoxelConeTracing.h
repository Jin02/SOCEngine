#pragma once

#include "ComputeShader.h"
#include "ConstBuffer.h"
#include "VXGICommon.h"
#include "VoxelMap.h"

#include "GaussianBlur.h"
#include "BilateralFiltering.h"

#include "ShaderManager.h"
#include "MainRenderer.h"

//#define USE_GAUSSIAN_BLUR

namespace Rendering
{
	namespace GI
	{
		class VoxelConeTracing
		{
		public:
			void Initialize(Device::DirectX& dx, Manager::ShaderManager& shaderMgr);
			void Run(Device::DirectX& dx, VoxelMap& injectionSourceMap, VoxelMap& mipmappedInjectionMap,
				VXGIInfoCB& infoCB, MainRenderingSystemParam& mainSystem);

			GET_ACCESSOR(IndirectColorMap, Texture::RenderTexture&, _indirectColorMap);

		private:
			Shader::ComputeShader		_shader;
			Texture::RenderTexture		_indirectColorMap;
		};
	}
}