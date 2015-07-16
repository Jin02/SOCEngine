#pragma once

#include "CameraForm.h"
#include "LightCulling.h"
#include "DirectX.h"

namespace Rendering
{
	namespace Camera
	{		
		class DeferredCamera : public CameraForm
		{
		public:
			static const Usage GetUsage() {	return Usage::MeshRender; }
			static const uint NumOfRenderTargets = 3;

		private:
			bool _useTransparent;

		private:
			Texture::RenderTexture*		_albedo_opacity;
			Texture::RenderTexture*		_normal_roughness;
			Texture::RenderTexture*		_specular_fresnel0;

			Texture::DepthBuffer*		_transparentDepthBuffer;
			Texture::DepthBuffer*		_opaqueDepthBuffer;

		public:
			DeferredCamera();
			virtual ~DeferredCamera(void);
		
		public:
			virtual void OnInitialize();
			virtual void OnDestroy();

		public:
			void Render(float dt, Device::DirectX* dx);

		public:
			void EnableRenderBlendedMesh(bool enable);

		public:
			virtual Core::Component* Clone() const;
		};

		typedef DeferredCamera Camera;
	}
}