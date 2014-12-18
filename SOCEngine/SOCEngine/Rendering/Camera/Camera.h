#pragma once

#include "DirectX.h"
#include "Component.h"
#include "Frustum.h"
#include "Structure.h"
#include "MeshManager.h"

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

		private:
			//사용하지 않음.
			//virtual void Render(){}
			//virtual void Update(float deltaTime){}
			//virtual void UpdateConstBuffer(const TransformPipelineParam& transpose_Transform){}

		public:
			void ProjectionMatrix(Math::Matrix &outMatrix);
			void ViewMatrix(Math::Matrix& outMatrix);

		public:
			virtual void Initialize();
			virtual void Destroy();

		public:
			void UpdateTransformAndCheckRender(const Structure::Vector<std::string, Core::Object>& objects);
			void RenderObjects(const Device::DirectX* dx, const Manager::MeshManager* meshMgr);
		};
	}
}