#pragma once

#include "Structure.h"
#include "CameraForm.h"

#include "LightCulling.h"

namespace Rendering
{
	namespace Manager
	{
		class CameraManager : public Structure::Vector<std::string, Camera::CameraForm>
		{
		private:
			Light::LightCulling*	_lightCullingCS;

		public:
			CameraManager(void);
			~CameraManager(void);

		public:
			void SetMainCamera(Camera::CameraForm* cam);
			Camera::CameraForm* GetMainCamera();

		public:
			GET_ACCESSOR(LightCullingCS, Light::LightCulling*, _lightCullingCS);
		}; 
	}
}