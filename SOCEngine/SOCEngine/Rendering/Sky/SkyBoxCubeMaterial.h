#pragma once

#include "Material.h"

namespace Rendering
{
	namespace Sky
	{
		class SkyBoxCubeMaterial : public Material
		{
		public:
			SkyBoxCubeMaterial(const std::string& name);
			virtual ~SkyBoxCubeMaterial();

		public:
			virtual void Initialize();
			virtual void Destroy();

			void UpdateCubeMap(const Rendering::Texture::Texture2D* tex);
		};
	}
}