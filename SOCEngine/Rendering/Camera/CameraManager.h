#pragma once

#include "Structure.h"
#include "Camera.h"

namespace Rendering
{
	class CameraManager : public Structure::Vector<Camera>
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