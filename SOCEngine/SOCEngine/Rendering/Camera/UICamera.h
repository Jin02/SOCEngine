#pragma once

#include "Camera.h"

namespace Rendering
{
	namespace Camera
	{		
		class UICamera : public Camera
		{
		public:
			static const Usage GetUsage() {	return Usage::UI; }

		public:
			UICamera();
			virtual ~UICamera(void);
		
		public:
			virtual void Initialize();
			virtual void Destroy();

		public:
			void Render();
		};
	}
}