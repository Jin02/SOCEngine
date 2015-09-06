#pragma once

#include "DirectX.h"
#include "Mesh.h"
#include "Frustum.h"
#include "Structure.h"
#include "RenderTexture.h"
#include "ConstBuffer.h"
#include "RenderManager.h"
#include "LightManager.h"

namespace Rendering
{
	namespace Camera
	{
		class CameraForm : public Core::Component
		{
		public:
			static const Core::Component::Type GetComponentType() {	return Core::Component::Type::Camera;	}

		public:
			enum class ProjectionType	{ Perspective, Orthographic };
			enum class Usage			{ MeshRender, UI };
			//enum ClearFlag { FlagSkybox, FlagSolidColor, FlagTarget, FlagDontClear };
			struct RenderQueue
			{
				uint								updateCounter;
				std::vector<const Mesh::Mesh*>		meshes;

				RenderQueue() : updateCounter(0){}
				~RenderQueue(){}
			};

		protected:
			Frustum*						_frustum;
			Texture::RenderTexture*			_renderTarget;
			RenderQueue						_transparentMeshQueue;
			
			Math::Matrix					_viewProjMatrixInPrevRenderState;

		protected:
			float							_fieldOfViewDegree;
			float							_clippingNear;
			float							_clippingFar;
			ProjectionType					_projectionType;
			float							_aspect;
			Color							_clearColor;

		public:
			CameraForm();
			virtual ~CameraForm(void);

		protected:
			void CalcAspect();
			void SortTransparentMeshRenderQueue(const Manager::RenderManager* renderMgr);

		public:
			void GetProjectionMatrix(Math::Matrix &outMatrix) const;

			static void  GetViewMatrix(Math::Matrix &outMatrix, const Math::Matrix &worldMatrix);
			void GetViewMatrix(Math::Matrix& outMatrix) const;

		protected:
			// if mainRTSampleCount = -1, mainRTSampleCount = msaa.count
			void Initialize(uint mainRTSampleCount = -1);
			void Destroy();

		public:
			void UpdateTransformCB(const std::vector<Core::Object*>& objects);
			virtual void Render(const Device::DirectX* dx, const Manager::RenderManager* renderManager, const Manager::LightManager* lightManager) = 0;

		protected:
			void _Clone(CameraForm* newCam) const;

		public:
			GET_SET_ACCESSOR(Near,				float, _clippingNear);
			GET_SET_ACCESSOR(Far,				float, _clippingFar);
			GET_SET_ACCESSOR(FieldOfViewDegree,	float, _fieldOfViewDegree);

			GET_ACCESSOR(ProjectionType, ProjectionType, _projectionType);
			GET_ACCESSOR(RenderTarget, const Texture::RenderTexture*, _renderTarget);
		};
	}
}