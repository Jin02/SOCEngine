#pragma once

#include <tuple>
#include "MainCamera.h"

namespace Core
{
	class Object;
}

namespace Rendering
{
	namespace Manager
	{
		template <class CameraType>
		using CameraPool = Core::VectorHashMap<Core::ObjectID::LiteralType, CameraType>;

		class CameraManager final
		{
		public:
			CameraManager();

		public:
			void Initialize(Device::DirectX& dx, Manager::ShaderManager& shaderMgr, const Rect<uint>& mainCamRenderRect);

			void SetMainCamera(Core::ObjectID objectID);
			GET_ACCESSOR(MainCamera, auto&, _mainCamera);

		public:
			template <class CameraType> CameraType& Acquire(Core::ObjectID objID)
			{
				return Add(CameraType(objID));
			}
			template <class CameraType> CameraType& Add(CameraType& camera)
			{
				camera.SetDirty(true);

				Core::ObjectID id = camera.GetObjectID();
				return GetPool<CameraType>().Add(id.Literal(), camera);
			}
			template <class CameraType> void Delete(Core::ObjectID objID)
			{
				auto& pool = GetPool<CameraType>();
				uint index = pool.GetIndexer().Find(objID.Literal());

				pool.Delete(objID.Literal());

				uint prevDeleteIDx = GetCameraDatas<CameraType>().reupdateMinIndex;
				GetCameraDatas<CameraType>().reupdateMinIndex = min(index, prevDeleteIDx);
			}
			template <class CameraType> bool Has(Core::ObjectID objID) const
			{
				return GetPool<CameraType>().Has(objID.Literal());
			}
			template <class CameraType> auto Find(Core::ObjectID id)
			{
				return GetPool<CameraType>().Find(id.Literal());
			}
			void DeleteAll();

			template <class CameraType> auto&		GetCameraPool()
			{
				return GetCameraDatas<CameraType>().pool;
			}
			template <class CameraType> const auto&	GetCameraPool() const
			{
				return GetCameraDatas<CameraType>().pool;
			}

		private:
			template <typename CameraType>
			struct CameraDatas
			{
				CameraPool<CameraType>		pool;
				std::vector<CameraType*>	dirty;
				uint						reupdateMinIndex = 0;
			};

			template <class CameraType> auto&		GetCameraDatas()
			{
				return std::get<CameraDatas<CameraType>>(_lightDatas);
			}
			template <class CameraType> const auto&	GetCameraDatas() const
			{
				return std::get<CameraDatas<CameraType>>(_lightDatas);
			}
			template <class CameraType> auto&		GetDirtyParamLights()
			{
				return GetCameraDatas<CameraType>().dirty;
			}
			template <class CameraType> auto&		GetDirtyTransformLights()
			{
				return GetCameraDatas<CameraType>().dirty;
			}


		private:
//			std::tuple<CameraDatas<>>				_cameras;
			Camera::MainCamera						_mainCamera;
		};
	}
}