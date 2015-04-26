#pragma once

#include "DirectX.h"
#include "Component.h"
#include "Frustum.h"
#include "Structure.h"
#include "RenderTexture.h"
#include "CameraConstBuffer.h"
#include "ConstBuffer.h"

namespace Rendering
{
	enum class RenderType
	{
		Unknown,
		TileBasedDeferred,
		ForwardPlus
	};

	namespace Camera
	{		
		class Camera : public Core::Component
		{
		public:
			static const Core::Component::Type GetComponentType() {	return Core::Component::Type::Camera;	}

		public:
			enum Type { Perspective, Orthographic };
			//enum ClearFlag { FlagSkybox, FlagSolidColor, FlagTarget, FlagDontClear };

		protected:
			Frustum					*_frustum;
			Texture::RenderTexture	*_renderTarget;
			Buffer::ConstBuffer		*_constBuffer;
			RenderType				 _renderType;

		protected:
			float				_FOV;
			float				_clippingNear;
			float				_clippingFar;
			//ClearFlag			_clearFlag;
			Type				_camType;
			float				_aspect;
			Color				_clearColor;

		public:
			Camera();
			virtual ~Camera(void);

		protected:
			void CalcAspect();

		public:
			void ProjectionMatrix(Math::Matrix &outMatrix);
			void ViewMatrix(Math::Matrix& outMatrix);

		public:
			virtual void Initialize();
			virtual void Destroy();

		public:
			void UpdateTransformCBAndCheckRender(const Structure::Vector<std::string, Core::Object>& objects);

		public:
			GET_SET_ACCESSOR(Near, float, _clippingNear);
			GET_SET_ACCESSOR(Far, float, _clippingFar);
			GET_SET_ACCESSOR(FOV, float, _FOV);
			GET_ACCESSOR(RenderType, RenderType, _renderType);
		};
	}
}