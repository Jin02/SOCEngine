#pragma once

#include "InjectRadianceUtility.h"
#include "ComputeShader.h"

namespace Rendering
{
	namespace GI
	{
		class InjectRadianceFromPointLight final
		{
		public:
			void Initialize(Device::DirectX& dx, Manager::ShaderManager& shaderMgr, uint dimension);
			void Inject(Device::DirectX& dx, VoxelMap& outVoxelMap,
				const Manager::LightManager& lightMgr,
				const Renderer::ShadowSystem& shadowParam,
				const InjectRadianceFormUtility::BindParam& bindParam);

		private:
			Shader::ComputeShader		_shader;
			uint						_threadLength = 0;
		};
	}
}