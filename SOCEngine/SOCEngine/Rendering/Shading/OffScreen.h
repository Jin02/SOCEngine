#pragma once

#include "FullScreen.h"

namespace Rendering
{
	namespace TBDR
	{
		class OffScreen : public PostProcessing::FullScreen
		{
		private:
			bool _useIndirectColorMap;

		public:
			OffScreen();
			virtual ~OffScreen();

		public:			
			void Initialize(const Texture::RenderTexture* directColorMap, bool useIndirectColorMap);
			void Render(const Device::DirectX* dx, const Texture::RenderTexture* outResultRT, const Texture::RenderTexture* indirectColorMap);
			void ReCompile(bool useIndirectColorMap);

		public:
			GET_ACCESSOR(UseIndirectColorMap, bool, _useIndirectColorMap);
		};
	}
}