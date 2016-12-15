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
			virtual void OnInitialize();
			virtual void OnDestroy();

		public:
			void Update(float delta);
			void Render(const Device::DirectX* dx);
			virtual void CullingWithUpdateCB(const Device::DirectX* dx, const std::vector<Core::Object*>& objects, const Manager::LightManager* lightManager, bool useHDR);

		public:
			virtual Core::Component* Clone() const;
		};
	}
}