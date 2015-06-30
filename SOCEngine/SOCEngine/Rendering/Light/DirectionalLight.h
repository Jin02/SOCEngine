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
				//dirZ는, LightTransformBuffer의 radius에 해당함

				Params() {}
				~Params() {}
			};

		public:
			DirectionalLight();
			~DirectionalLight();

		public:
			virtual bool Intersects(const Intersection::Sphere &sphere);
			void MakeLightBufferElement(LightTransformBuffer& outTransform, Params& outParam) const;
		};
	}
}