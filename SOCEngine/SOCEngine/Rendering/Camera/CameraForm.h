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
			struct CamMatCBData
			{
				Math::Matrix viewMat;
				Math::Matrix viewProjMat;
			};

		public:
			enum class ProjectionType	{ Perspective, Orthographic };
			enum class Usage			{ MeshRender, UI };
			//enum ClearFlag { FlagSkybox, FlagSolidColor, FlagTarget, FlagDontClear };
			struct RenderQueue
			{
				uint									updateCounter;
				std::vector<const Geometry::Mesh*>		meshes;

				RenderQueue() : updateCounter(0){}
				~RenderQueue(){}
			};

		private:
			const Usage _usage;

		protected:
			Intersection::Frustum*			_frustum;
			Texture::RenderTexture*			_renderTarget;
			RenderQueue						_transparentMeshQueue;

			float							_fieldOfViewDegree;
			float							_clippingNear;
			float							_clippingFar;
			ProjectionType					_projectionType;
			float							_aspect;
			Color							_clearColor;

			Buffer::ConstBuffer*			_camMatConstBuffer;
			CamMatCBData					_prevCamMatCBData;

			Math::Rect<float>				_renderRect;

		public:
			CameraForm(Usage usage);
			virtual ~CameraForm(void);

		protected:
			void CalcAspect();
			void SortTransparentMeshRenderQueue(const Manager::RenderManager* renderMgr);

		public:
			void GetPerspectiveMatrix(Math::Matrix &outMatrix, bool isInverted) const;
			void GetOrthogonalMatrix(Math::Matrix &outMatrix, bool isInverted, const Math::Size<uint>* customWH = nullptr) const;

			void GetProjectionMatrix(Math::Matrix &outMatrix, bool isInverted) const;

			static void  GetViewMatrix(Math::Matrix &outMatrix, const Math::Matrix &worldMatrix);
			void GetViewMatrix(Math::Matrix& outMatrix) const;

			static void GetViewportMatrix(Math::Matrix& outMat, const Math::Rect<float>& rect);
			static void GetInvViewportMatrix(Math::Matrix& outMat, const Math::Rect<float>& rect);

		protected:
			// if mainRTSampleCount = 0, mainRTSampleCount = msaa.count
			void Initialize(const Math::Rect<float>& renderRect, uint mainRTSampleCount = 0);
			void Destroy();

		public:
			virtual void CullingWithUpdateCB(const Device::DirectX* dx, const std::vector<Core::Object*>& objects, const Manager::LightManager* lightManager);

		protected:
			void _Clone(CameraForm* newCam) const;

		public:
			GET_SET_ACCESSOR(Near,				float,						_clippingNear);
			GET_SET_ACCESSOR(Far,				float,						_clippingFar);
			GET_SET_ACCESSOR(FieldOfViewDegree,	float,						_fieldOfViewDegree);
			GET_SET_ACCESSOR(RenderRect,		const Math::Rect<float>&,	_renderRect);

			GET_ACCESSOR(ProjectionType, ProjectionType, _projectionType);
			GET_ACCESSOR(RenderTarget, const Texture::RenderTexture*, _renderTarget);

			GET_ACCESSOR(Usage, const Usage, _usage);
		};
	}
}