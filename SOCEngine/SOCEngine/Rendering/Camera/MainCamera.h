#pragma once

#include "CameraForm.h"
#include "ShadingWithLightCulling.h"
#include "OnlyLightCulling.h"
#include "OffScreen.h"

namespace Rendering
{
	namespace Camera
	{		
		class MainCamera : public CameraForm
		{
		public:
			static const Usage GetUsage() {	return Usage::MeshRender; }
			static const uint NumOfRenderTargets = 3;

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
			MainCamera();
			virtual ~MainCamera(void);
		
		public:
			virtual void OnInitialize();
			virtual void OnDestroy();

		public:
			virtual void CullingWithUpdateCB(const Device::DirectX* dx, const std::vector<Core::Object*>& objects, const Manager::LightManager* lightManager);
			virtual void Render(const Device::DirectX* dx, const Manager::RenderManager* renderManager, const Manager::LightManager* lightManager);

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
		typedef Camera::MainCamera MainRenderer;
	}
}