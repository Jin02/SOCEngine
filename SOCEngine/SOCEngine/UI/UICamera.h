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

		public:
			UICamera();
			virtual ~UICamera(void);
		
		public:
			virtual void Initialize();
			virtual void Destroy();

		public:
			void Update(float delta);
			virtual void Render(const Device::DirectX* dx, const Manager::RenderManager* renderManager, const Manager::LightManager* lightManager);

		public:
			virtual Core::Component* Clone() const;
		};
	}
}