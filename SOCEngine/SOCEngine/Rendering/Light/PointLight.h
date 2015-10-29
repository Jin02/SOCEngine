#pragma once

#include "LightForm.h"
#include <array>

namespace Rendering
{
	namespace Light
	{
		class PointLight : public LightForm
		{
		private:
			Math::Matrix	_viewMat[5];		// another viewMat was placed in LightForm.
			Math::Matrix	_viewProjMat[5];	// another viewProjMat was placed in LightForm.

			Math::Matrix	_prevViewProj;

		public:
			PointLight();
			~PointLight();

		public:
			virtual void ComputeViewProjMatrix(const Intersection::BoundBox& sceneBoundBox);

		public:
			bool Intersect(const Intersection::Sphere &sphere) const;
			void MakeLightBufferElement(LightTransformBuffer& out) const;

		public:
			virtual Core::Component* Clone() const;

		public:
			void GetViewMatrices(std::array<Math::Matrix, 6>& out) const;
			void GetViewProjectionMatrices(std::array<Math::Matrix, 6>& out) const;
		};

	}
}