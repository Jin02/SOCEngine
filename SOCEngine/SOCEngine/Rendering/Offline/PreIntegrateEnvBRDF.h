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
			Texture::Texture2D& CreatePreBRDFMap(Device::DirectX& dx, Manager::ShaderManager& shaderMgr);
			GET_ACCESSOR(Texture2D, Texture::Texture2D&, _texture);

		private:
			Texture::Texture2D		_texture;
		};
	}
}