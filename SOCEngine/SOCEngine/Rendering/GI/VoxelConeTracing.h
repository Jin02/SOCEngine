#pragma once

#include "ComputeShader.h"
#include "ConstBuffer.h"
#include "VXGICommon.h"
#include "VoxelMap.h"
#include "GaussianBlur.h"
#include "BilateralFiltering.h"
#include "ShaderManager.h"

//#define USE_GAUSSIAN_BLUR

namespace Rendering
{
	namespace GI
	{
		class VoxelConeTracing
		{
		public:
			void Initialize(Device::DirectX& dx, Manager::ShaderManager& shaderMgr);

			struct Param
			{
				const VoxelMap&						injectionSourceMap;
				const VoxelMap&						mipmappedInjectionMap;
				const VXGIInfoCB&					infoCB;
				const MainRenderingSystemParam&		mainSystem;
			};
			void Run(Device::DirectX& dx, Texture::RenderTexture& outIndirectColorMap, const Param&& param);

		private:
			Shader::ComputeShader					_shader;
			Shader::ComputeShader::ThreadGroup		_group;
		};
	}
}