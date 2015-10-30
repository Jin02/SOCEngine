#pragma once

#include "CameraForm.h"
#include "ShadingWithLightCulling.h"
#include "OnlyLightCulling.h"
#include "OffScreen.h"
#include <functional>

namespace Rendering
{
	namespace Camera
	{		
		class MeshCamera : public CameraForm
		{
		public:
			static const Usage GetUsage() {	return Usage::MeshRender; }
			static const uint NumOfRenderTargets = 3;
			enum class RenderType
			{
				AlphaMesh,
				Opaque,
				Transparency,
				DepthOnly
			};

		private:
			bool _useTransparent;
			Light::LightCulling::TBRParam				_prevParamData;
			Buffer::ConstBuffer*						_tbrParamConstBuffer;

		private:
			Texture::RenderTexture*						_albedo_emission;
			Texture::RenderTexture*						_normal_roughness;
			Texture::RenderTexture*						_specular_metallic;

			TBDR::ShadingWithLightCulling*				_deferredShadingWithLightCulling;
			Texture::DepthBuffer*						_opaqueDepthBuffer;

			Light::OnlyLightCulling*					_blendedMeshLightCulling;
			Texture::DepthBuffer*						_blendedDepthBuffer;
			TBDR::OffScreen*							_offScreen;


		public:
			MeshCamera();
			virtual ~MeshCamera(void);
		
		public:
			virtual void OnInitialize();
			virtual void OnDestroy();

		public:
			virtual void CullingWithUpdateCB(const Device::DirectX* dx, const std::vector<Core::Object*>& objects, const Manager::LightManager* lightManager);
			void Render(const Device::DirectX* dx, const Manager::RenderManager* renderManager, const Manager::LightManager* lightManager, const Buffer::ConstBuffer* shadowGlobalParamCB);

		public:
			static void RenderMeshWithoutIASetVB(
				const Device::DirectX* dx, const Manager::RenderManager* renderManager,
				const Geometry::Mesh* mesh, RenderType renderType,
				const Buffer::ConstBuffer* cameraConstBuffer);
			static void RenderMeshesUsingSortedMeshVectorByVB(
				const Device::DirectX* dx, const Manager::RenderManager* renderManager,
				const Manager::RenderManager::MeshList& meshes,
				RenderType renderType, const Buffer::ConstBuffer* cameraConstBuffer,
				std::function<bool(const Intersection::Sphere&)>* intersectFunc = nullptr, const Intersection::Frustum* customFrustum = nullptr);
			static void RenderMeshesUsingMeshVector(
				const Device::DirectX* dx, const Manager::RenderManager* renderManager,
				const std::vector<const Geometry::Mesh*>& meshes,
				RenderType renderType, const Buffer::ConstBuffer* cameraConstBuffer,
				std::function<bool(const Intersection::Sphere&)>* intersectFunc = nullptr, const Intersection::Frustum* customFrustum = nullptr);

		public:
			void EnableRenderTransparentMesh(bool enable);

		public:
			virtual Core::Component* Clone() const;

		public:
			GET_ACCESSOR(TBRParamConstBuffer, const Buffer::ConstBuffer*, _tbrParamConstBuffer);
		};
	}

	namespace Renderer
	{
		typedef Camera::MeshCamera MainRenderer;
	}
}