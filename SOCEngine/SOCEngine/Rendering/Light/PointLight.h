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
		public:
			struct Param{};

		public:
			PointLight();
			virtual ~PointLight();

		public:
			virtual void CreateShadow();

		public:
			bool Intersect(const Intersection::Sphere &sphere) const;
			void MakeLightBufferElement(LightTransformBuffer& out, Param& dummyParam) const;

		public:
			virtual Core::Component* Clone() const;

		public:
			GET_ACCESSOR(Shadow, Shadow::PointLightShadow*, static_cast<Shadow::PointLightShadow*>(_shadow) );
		};

	}
}