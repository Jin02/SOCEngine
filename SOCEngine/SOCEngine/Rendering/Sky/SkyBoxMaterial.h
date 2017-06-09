#pragma once

#include "DirectX.h"
#include "Matrix.h"
#include "ShaderManager.h"
#include "Material.hpp"

namespace Rendering
{
	class SkyBoxMaterial : public Material
	{
	public:
		using Material::Material;
		void UpdateCubeMap(TextureBindIndex bind, const Texture::Texture2D& tex);
	};
}
