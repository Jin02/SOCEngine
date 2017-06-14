#pragma once

#include "DirectX.h"
#include "Matrix.h"
#include "ShaderManager.h"
#include "Material.hpp"

namespace Rendering
{
	namespace Material
	{
		class SkyBoxMaterial : public MaterialForm
		{
		public:
			using MaterialForm::MaterialForm;

			void UpdateCubeMap(const Texture::Texture2D& tex);
			GET_ACCESSOR(CubeMap, auto&, _cubeMap);

		private:
			Texture::Texture2D _cubeMap;
		};
	}
}
