#pragma once

#include "DirectX.h"

namespace Rendering
{
	namespace Renderer
	{
		class TransparencyRenderer
		{
		protected:

		public:
			TransparencyRenderer();
			~TransparencyRenderer();

		public:
			void Initialize();
			void Destroy();

		public:
			void Render(ID3D11RenderTargetView* outResultRT);
		};
	}
}