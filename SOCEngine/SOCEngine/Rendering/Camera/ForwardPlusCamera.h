#pragma once

#include "Camera.h"

namespace Rendering
{
	namespace Camera
	{		
		class ForwardPlusCamera : public Camera
		{
		public:
			static const Usage GetUsage() {	return Usage::MeshRender; }

		private:
			Texture::DepthBuffer*		_opaqueDepthBuffer;
			Texture::DepthBuffer*		_transparentDepthBuffer;

			bool						_useTransparentRender;

		public:
			ForwardPlusCamera();
			virtual ~ForwardPlusCamera(void);
		
		public:
			virtual void Initialize();
			virtual void Destroy();

		public:
			void Render();

		public:
			GET_SET_ACCESSOR(UseTransparent, bool, _useTransparentRender);
		};
	}
}