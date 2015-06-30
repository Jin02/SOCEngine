#pragma once

#include "CameraForm.h"

namespace Rendering
{
	namespace Camera
	{		
		class UICamera : public CameraForm
		{
		public:
			static const Usage GetUsage() {	return Usage::UI; }

		private:
			Texture::DepthBuffer*		_depthBuffer;
			Core::Object*				_object;

		public:
			UICamera();
			virtual ~UICamera(void);
		
		public:
			virtual void Initialize();
			virtual void Destroy();

		public:
			void Update(float delta);
			void Render();

		public:
			GET_ACCESSOR(Object, Core::Object*, _object);
		};
	}
}