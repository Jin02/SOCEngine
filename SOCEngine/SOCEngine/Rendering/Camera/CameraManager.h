#pragma once

#include "Structure.h"
#include "Camera.h"

namespace Rendering
{
	namespace Manager
	{
		class CameraManager : public Structure::Vector<std::string, Camera::Camera>
		{
		public:
			CameraManager(void);
			~CameraManager(void);

		public:
			void SetMainCamera(Camera::Camera* cam);
			Camera::Camera* GetMainCamera();

		public:
			void Render(std::vector<Core::Object*>::iterator& objectBegin,
				std::vector<Core::Object*>::iterator& objectEnd, LightManager* sceneLights);
		}; 
	}
}