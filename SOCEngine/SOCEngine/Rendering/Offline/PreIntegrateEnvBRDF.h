#pragma once

#include "DirectX.h"
#include "Texture2D.h"
#include "ShaderManager.h"

namespace Rendering
{
	namespace Precompute
	{
		class PreIntegrateEnvBRDF final
		{
		public:
			PreIntegrateEnvBRDF();
			const Texture::Texture2D& CreatePreBRDFMap(Device::DirectX& dx, Manager::ShaderManager& shaderMgr);

			GET_CONST_ACCESSOR(PreIntegrateEnvBRDFMap, const Texture::Texture2D&, _texture);

		private:
			Texture::Texture2D		_texture;
		};
	}
}