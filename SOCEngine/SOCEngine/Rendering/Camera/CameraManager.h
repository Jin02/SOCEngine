#pragma once

#include <tuple>
#include "MainCamera.h"

template <class CameraType>
using CameraPool = Core::VectorHashMap<Core::ObjectId::LiteralType, CameraType>;

namespace Core
{
	class Object;
}

namespace Rendering
{
	namespace Manager
	{
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
			CameraManager();

		public:
			void Initialize(Device::DirectX& dx, Manager::ShaderManager& shaderMgr, const Rect<uint>& mainCamRenderRect);
			void SetMainCamera(Core::Object object);

		public:
			template <class CameraType>
			CameraType& Acquire(Core::ObjectId objId)
			{
			}

			template <class CameraType>
			void Delete(Core::ObjectId objId)
			{
			}

			template <class CameraType>
			bool Has(Core::ObjectId objId)
			{

			}

			template <class Component>
			auto Find(Core::ObjectId id)
			{

			}

		public:
			GET_ACCESSOR(MainCamera, auto&, _mainCamera);

		private:
//			CameraSystem<Camera::MeshCamera>		_cameraSystem;
			Camera::MainCamera						_mainCamera;
		};
	}
}