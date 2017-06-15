#pragma once

#include "InjectRadianceUtility.h"
#include "ComputeShader.h"

namespace Rendering
{
	namespace GI
	{
		class InjectRadianceFromSpotLight final
		{
		public:
			void Initialize(Device::DirectX& dx, Manager::ShaderManager& shaderMgr, uint dimension);
			void Inject(Device::DirectX& dx, Manager::LightManager& lightMgr, ShadowSystemParam& shadowSystem, InjectRadianceFormUtility::BindParam& bindParam);

		private:
			Shader::ComputeShader	_shader;
		};
	}
}