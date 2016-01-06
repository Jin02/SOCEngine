#pragma once

#include "Structure.h"
#include "CameraForm.h"

namespace Rendering
{
	namespace Manager
	{
		class CameraManager : public Structure::VectorMap<std::string, Camera::CameraForm*>
		{
		public:
			CameraManager(void);
			~CameraManager(void);

		public:
			void SetFirstCamera(Camera::CameraForm* cam);
			Camera::CameraForm* GetFirstCamera() const;
		}; 
	}
}