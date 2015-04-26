#pragma once

#include "Camera.h"

namespace Rendering
{
	namespace Camera
	{		
		class ForwardPlusCamera : public Camera
		{
		private:
			Texture::DepthBuffer*		_opaqueDepthBuffer;
			Texture::DepthBuffer*		_transparentDepthBuffer;

		public:
			ForwardPlusCamera();
			virtual ~ForwardPlusCamera(void);
		
		public:
			virtual void Initialize();
			virtual void Destroy();

		public:
			
		};
	}
}