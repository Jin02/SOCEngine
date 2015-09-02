#pragma once

#include "DirectX.h"
#include "Mesh.h"
#include "Frustum.h"
#include "Structure.h"
#include "RenderTexture.h"
#include "ConstBuffer.h"

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
			struct ConstBufferParam
			{
				Math::Vector4 worldPos;
				float clippingNear, clippingFar;
				Math::Size<float> screenSize;
			};

		private:
			ConstBufferParam				_prevConstBufferData;

		protected:
			Frustum*						_frustum;
			Texture::RenderTexture*			_renderTarget;
			Buffer::ConstBuffer*			_camConstBuffer;
			RenderQueue						_transparentMeshQueue;
			
			Math::Matrix					_viewProjMatrixInPrevRenderState;

		protected:
			bool							_isInvertedDepthWriting;
			float							_FOV;
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
			void SortTransparentMeshRenderQueue();

		public:
			void GetProjectionMatrix(Math::Matrix &outMatrix) const;

			static void  GetViewMatrix(Math::Matrix &outMatrix, const Math::Matrix &worldMatrix);
			void GetViewMatrix(Math::Matrix& outMatrix) const;

		public:
			virtual void OnInitialize();
			virtual void OnDestroy();

		public:
			void RenderPreviewWithUpdateTransformCB(const std::vector<Core::Object*>& objects);
			virtual void Render(float dt, Device::DirectX* dx) = 0;

		protected:
			void _Clone(CameraForm* newCam) const;

		public:
			GET_SET_ACCESSOR(Near, float, _clippingNear);
			GET_SET_ACCESSOR(Far, float, _clippingFar);
			GET_SET_ACCESSOR(FOV, float, _FOV);
			GET_SET_ACCESSOR(IsInvertedDepthWriting, bool, _isInvertedDepthWriting);

			GET_ACCESSOR(ProjectionType, ProjectionType, _projectionType);
			GET_ACCESSOR(RenderTarget, const Texture::RenderTexture*, _renderTarget);
		};
	}
}