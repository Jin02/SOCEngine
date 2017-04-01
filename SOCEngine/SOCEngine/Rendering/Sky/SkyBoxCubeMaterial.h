#pragma once

#include "SkyBoxMaterial.h"

namespace Rendering
{
	namespace Sky
	{
		class SkyBoxCubeMaterial : public SkyBoxMaterial
		{
		public:
			using SkyBoxMaterial::SkyBoxMaterial;
			void Initialize(Device::DirectX& dx, Manager::ShaderManager& shaderMgr);

		private:
			using SkyBoxMaterial::Initialize;
		};
	}
}