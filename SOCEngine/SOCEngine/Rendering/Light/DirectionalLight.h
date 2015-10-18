#pragma once

#include "LightForm.h"

namespace Rendering
{
	namespace Light
	{
		class DirectionalLight : public LightForm
		{
		public:
			struct Params
			{
				unsigned short dirX;
				unsigned short dirY;
				//dirZ��, LightTransformBuffer�� radius�� �ش���

				Params() {}
				~Params() {}
			};

		public:
			DirectionalLight();
			~DirectionalLight();

		public:
			bool Intersect(const Intersection::Sphere &sphere) const;
			void MakeLightBufferElement(LightTransformBuffer& outTransform, Params& outParam) const;

		public:
			virtual Core::Component* Clone() const;
		};
	}
}