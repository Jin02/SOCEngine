#pragma once

#include "LightForm.h"
#include <array>
#include "PointLightShadow.h"

namespace Rendering
{
	namespace Light
	{
		class PointLight : public LightForm
		{
		private:
			Math::Matrix	_viewMatOffsetOne[5];		// another viewMat was placed in LightForm.
			Math::Matrix	_viewProjMatOffsetOne[5];	// another viewProjMat was placed in LightForm.

			Math::Matrix	_prevViewProj;

		public:
			PointLight();
			virtual ~PointLight();

		public:
			virtual void ComputeViewProjMatrix(const Intersection::BoundBox& sceneBoundBox);
			virtual void CreateLightShadow(const std::function<void()>& addUpdateCounter);

		public:
			bool Intersect(const Intersection::Sphere &sphere) const;
			void MakeLightBufferElement(LightTransformBuffer& out) const;

		public:
			virtual Core::Component* Clone() const;

		public:
			void GetViewMatrices(std::array<Math::Matrix, 6>& out) const;
			void GetViewProjectionMatrices(std::array<Math::Matrix, 6>& out) const;

			GET_ACCESSOR(Shadow, const Shadow::PointLightShadow*, static_cast<const Shadow::PointLightShadow*>(_shadow) );
		};

	}
}