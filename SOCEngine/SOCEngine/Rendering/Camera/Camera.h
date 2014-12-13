#pragma once

#include "DirectX.h"
#include "Component.h"
#include "Frustum.h"
#include "LightManager.h"
#include "Component.h"
#include "Structure.h"

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
			enum ClearFlag { FlagSkybox, FlagSolidColor, FlagTarget, FlagDontClear };
			//CLEAR_FLAG_DEPTHONLY는 제외함. 어떻게 구현하라는건지 잘 모르겠음 -ㅠ-;

		private:
			Frustum					*_frustum;
			//Shader::Shader			*rtShader;
			//Texture::RenderTarget	*renderTarget;

		private:
			float				_FOV;
			float				_clippingNear;
			float				_clippingFar;
			ClearFlag			_clearFlag;
			Type				_camType;
			float				_aspect;
			Color				_clearColor;

		public:
			Camera();
			~Camera(void);

		private:
			void CalcAspect();
			void RenderObjects(const Device::DirectX* dx, const Structure::Vector<Core::Object>& objects);

		public:
			void ProjectionMatrix(Math::Matrix &outMatrix);
			void ViewMatrix(Math::Matrix& outMatrix);

		public:
			//static void SceneUpdate(float dt, std::vector<Object*> *sceneObjects);
			void Render( const Structure::Vector<Core::Object>& objects);

		public:
			virtual void Initialize();
			virtual void Destroy();
			//		virtual Component::Type GetComponentType();
		};
	}
}