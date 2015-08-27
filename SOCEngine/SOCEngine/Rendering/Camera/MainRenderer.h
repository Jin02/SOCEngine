#pragma once

#include "CameraForm.h"
#include "ShadingWithLightCulling.h"
#include "DirectX.h"
#include "OnlyLightCulling.h"
#include "OffScreen.h"

namespace Rendering
{
	namespace Camera
	{		
		class MainRenderer : public CameraForm
		{
		public:
			static const Usage GetUsage() {	return Usage::MeshRender; }
			static const uint NumOfRenderTargets = 3;

		private:
			bool _useTransparent;
			Light::LightCulling::TBRChangeableParam		_prevParamData;
			Buffer::ConstBuffer* _tbrParamConstBuffer;

		private:
			Texture::RenderTexture*						_albedo_metallic;
			Texture::RenderTexture*						_normal_roughness;
			Texture::RenderTexture*						_specular_fresnel0;

			DeferredShading::ShadingWithLightCulling*	_deferredShadingWithLightCulling;
			Texture::DepthBuffer*						_opaqueDepthBuffer;

			Light::OnlyLightCulling*					_blendedMeshLightCulling;
			Texture::DepthBuffer*						_blendedDepthBuffer;
			DeferredShading::OffScreen*					_offScreen;


		public:
			MainRenderer();
			virtual ~MainRenderer(void);
		
		public:
			virtual void OnInitialize();
			virtual void OnDestroy();

		public:
			void Render(float dt, Device::DirectX* dx);

		public:
			void EnableRenderTransparentMesh(bool enable);

		public:
			virtual Core::Component* Clone() const;
		};

		typedef MainRenderer Camera;
	}
}