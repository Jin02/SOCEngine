#pragma once

#include "BaseLight.h"
#include "Transform.h"
#include "Half.h"

namespace Rendering
{
	namespace Light
	{
		class DirectionalLight final
		{
		public:
			using TransformType = std::pair<Half, Half>;
			DirectionalLight(Core::ObjectId);

			void UpdateFlag(const Core::Transform& transform);
//			void ComputeViewProjMatrix(const Core::Transform& transform, const Intersection::BoundBox& sceneBoundBox, const Math::Matrix& invViewportMat);

			void MakeTransform(TransformType& outDir, const Core::Transform& transform) const;

			bool Intersect(const Intersection::Sphere &sphere, const Core::Transform& transform) const { return true; }

		public:
//			GET_ACCESSOR(ViewProjectionMatrix,				const Math::Matrix&,				_viewProjMat);
//			GET_SET_ACCESSOR(ProjectionSize,				float,								_projectionSize);
//			GET_SET_ACCESSOR(UseAutoProjectionLocation,		bool,								_useAutoProjectLocation);
			GET_CONST_ACCESSOR(LightShaftSize,				float,								_base.GetRadius());
			inline void SetLightShaftSize(float f) { _base.SetRadius(f); }

			GET_ACCESSOR(Base, BaseLight&, _base);
			GET_CONST_ACCESSOR(ObjectId, const Core::ObjectId&, _base.GetObjectId());

		private:
			BaseLight					_base;
//			Math::Matrix				_viewProjMat;
//			float						_projectionSize = 0.0f;
//			bool						_useAutoProjectLocation = true;
		};
	}
}
