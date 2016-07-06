#pragma once

#include "Texture2D.h"

namespace Rendering
{
	namespace Precompute
	{
		class PreIntegrateEnvBRDF
		{
		private:
			Texture::Texture2D*	_map;

		public:
			PreIntegrateEnvBRDF();
			~PreIntegrateEnvBRDF();

		public:
			static Texture::Texture2D* CreatePreBRDFMap();
			Texture::Texture2D* FetchPreBRDFMap();

		public:
			GET_ACCESSOR(PreIntegrateEnvBRDFMap, const Texture::Texture2D*, _map);
		};
	}
}