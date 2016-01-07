#pragma once

#include "Structure.h"
#include "CameraForm.h"

namespace Rendering
{
	namespace Manager
	{
		class CameraManager : private Structure::VectorMap<address, Camera::CameraForm*>
		{
		public:
			CameraManager(void);
			~CameraManager(void);

		public:
			void Add(Camera::CameraForm* camera);
			void Delete(Camera::CameraForm* camera);
			void DeleteAll();
			void Destroy();

			void SetMainCamera(Camera::CameraForm* cam);
			Camera::CameraForm* GetMainCamera() const;

		public:
			GET_ACCESSOR(CameraVector, const std::vector<Camera::CameraForm*>&, _vector);
		}; 
	}
}