#pragma once

#include "MeshCamera.h"
#include <tuple>

namespace Rendering
{
	namespace Manager
	{
		template <class CameraType>
		class CameraPool : public Core::VectorHashMap<Core::ObjectId::LiteralType, CameraType> {};

		template <class... CameraTypes>
		class CameraSystem final
		{
		public:
			CameraSystem() = default;

			template <typename CameraType>
			void Add(Core::ObjectId key, CameraType& camera)
			{
				GetPool<CameraType>().Add(key, camera);
			}
			template <typename CameraType>
			void Delete(Core::ObjectId key)
			{
				std::get<CameraPool<CameraType>>(_cameras).Delete(key);
			}
			template <typename CameraType>
			auto Find(Core::ObjectId key)
			{
				return std::get<CameraPool<CameraType>>(_cameras).Find(key);
			}
			template <typename CameraType>
			bool Has(Core::ObjectId key) const
			{
				return std::get<CameraPool<CameraType>>(_cameras).GetIndexer().Has(key);
			}

			template <typename CameraType>
			const std::vector<CameraType>& GetCameras() const
			{
				return std::get<CameraPool<CameraType>>(_cameras).GetVector();
			}

			template <typename CameraType>
			CameraPool<CameraType>& GetPool()
			{
				return std::get<CameraPool<CameraType>>(_cameras);
			}

		private:
			std::tuple<CameraPool<CameraTypes>...>	_cameras;
		}; 

		class CameraManager final
		{
		public:
			CameraManager() = default;
			DISALLOW_ASSIGN_COPY(CameraManager);
			auto& Get()
			{
				return _cameraSystem;
			}

			std::shared_ptr<Camera::MeshCamera> GetMainMeshCamera();
			void SetMainMeshCamera(Camera::MeshCamera& cam);

		private:
			CameraSystem<Camera::MeshCamera>		_cameraSystem;
			Core::ObjectId							_mainMeshCamId;
		};
	}
}