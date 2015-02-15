#pragma once

#include "Structure.h"
#include "Camera.h"

#include "LightCulling.h"

namespace Rendering
{
	namespace Manager
	{
		class CameraManager : public Structure::Vector<std::string, Camera::Camera>
		{
		private:
			Light::LightCulling*	_lightCullingCS;

		public:
			CameraManager(void);
			~CameraManager(void);

		public:
			void InitLightCulling();
			void SetMainCamera(Camera::Camera* cam);
			Camera::Camera* GetMainCamera();

		public:
			GET_ACCESSOR(LightCullingCS, Light::LightCulling*, _lightCullingCS);
		}; 
	}
}