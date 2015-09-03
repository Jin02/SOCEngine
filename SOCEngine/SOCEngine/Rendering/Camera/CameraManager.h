#pragma once

#include "Structure.h"
#include "CameraForm.h"

#include "LightCulling.h"

namespace Rendering
{
	namespace Manager
	{
		class CameraManager : public Structure::VectorMap<std::string, Camera::CameraForm*>
		{
		private:
			Light::LightCulling*	_lightCullingCS;

		public:
			CameraManager(void);
			~CameraManager(void);

		public:
			void SetFirstCamera(Camera::CameraForm* cam);
			Camera::CameraForm* GetFirstCamera() const;

		public:
			GET_ACCESSOR(LightCullingCS, Light::LightCulling*, _lightCullingCS);
		}; 
	}
}