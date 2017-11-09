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
			void Run(	Device::DirectX& dx,
						const VoxelMap& injectionSourceMap, const VoxelMap& mipmappedInjectionMap,
						const VXGIInfoCB& infoCB, const MainRenderingSystemParam& mainSystem	);

			GET_CONST_ACCESSOR(IndirectColorMap, const auto&, _indirectColorMap.GetTexture2D());

		private:
			Shader::ComputeShader		_shader;
			Texture::RenderTexture		_indirectColorMap;
		};
	}
}