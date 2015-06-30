#pragma once

#include "CameraForm.h"
#include "LightCulling.h"

namespace Rendering
{
	namespace Camera
	{		
		class ForwardPlusCamera : public CameraForm
		{
		public:
			static const Usage GetUsage() {	return Usage::MeshRender; }

		private:
			Texture::DepthBuffer*		_opaqueDepthBuffer;
			Texture::DepthBuffer*		_blendedDepthBuffer;

			Light::LightCulling*		_lightCulling;

		public:
			ForwardPlusCamera();
			virtual ~ForwardPlusCamera(void);
		
		public:
			virtual void OnInitialize();
			virtual void OnDestroy();

		public:
			void Render();

		public:
			void EnableRenderBlendedMesh(bool enable);

		public:
			GET_ACCESSOR(IsRenderBlendedMesh, bool, (_blendedDepthBuffer != nullptr));
		};

		typedef ForwardPlusCamera Camera;
	}
}