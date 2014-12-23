#pragma once

#include "DirectX.h"
#include "Component.h"
#include "Frustum.h"
#include "Structure.h"
#include "RenderManager.h"
#include "RenderTexture.h"

namespace Rendering
{
	namespace Camera
	{
		class Camera : public Core::Component
		{
		public:
			static const Core::Component::Type GetComponentType() {	return Core::Component::Type::Camera;	}

		public:
			enum Type { Perspective, Orthographic };
			//enum ClearFlag { FlagSkybox, FlagSolidColor, FlagTarget, FlagDontClear };

		private:
			Frustum					*_frustum;
			Texture::RenderTexture	*_depthBuffer;
			Texture::RenderTexture	*_renderTarget;

		private:
			float				_FOV;
			float				_clippingNear;
			float				_clippingFar;
			//ClearFlag			_clearFlag;
			Type				_camType;
			float				_aspect;
			Color				_clearColor;

		public:
			Camera();
			~Camera(void);

		private:
			void CalcAspect();

		public:
			void ProjectionMatrix(Math::Matrix &outMatrix);
			void ViewMatrix(Math::Matrix& outMatrix);

		public:
			virtual void Initialize();
			virtual void Destroy();

		public:
			void UpdateTransformAndCheckRender(const Structure::Vector<std::string, Core::Object>& objects);
			void RenderObjects(const Device::DirectX* dx, const Manager::RenderManager* renderMgr);
		};
	}
}