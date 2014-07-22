#pragma once

#include "Container.h"
#include "Camera.h"

namespace Rendering
{
	class CameraManager : public Container<Camera>
	{
	public:
		CameraManager(void);
		~CameraManager(void);

	public:
		void SetMainCamera(Camera* cam);
		Camera* GetMainCamera();

	public:
		void Render(std::vector<Core::Object*>::iterator& objectBegin,
			std::vector<Core::Object*>::iterator& objectEnd, Light::LightManager* sceneLights);
	}; 
}