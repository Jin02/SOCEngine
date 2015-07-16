#pragma once

#include "LightCulling.h"

namespace Rendering
{
	namespace Light
	{
		class DeferredShadingWithLightCulling : private LightCulling
		{
		private:

		public:
			DeferredShadingWithLightCulling();
			~DeferredShadingWithLightCulling();

		public:
			void Initialize();

		};
	}
}