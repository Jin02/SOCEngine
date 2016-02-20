#pragma once

#include "MeshFilter.h"
#include "MeshRenderer.h"

namespace Rendering
{
	namespace Sky
	{
		class SkyDome
		{
		private:
			Geometry::MeshFilter*		_filter;
			Geometry::MeshRenderer*		_renderer;

		public:
			SkyDome();
			~SkyDome();

		public:
			void Initialize(const std::string& domeTexturePath);
		};
	}
}