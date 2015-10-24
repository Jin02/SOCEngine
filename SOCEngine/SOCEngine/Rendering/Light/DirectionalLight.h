#pragma once

#include "LightForm.h"
#include "Frustum.h"

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

		private:
			Intersection::Frustum		_frustum;

		public:
			DirectionalLight();
			~DirectionalLight();

		public:
			inline void ComputeFrustum(const Math::Matrix& viewProjMat) { _frustum.Make(viewProjMat); }

			bool Intersect(const Intersection::Sphere &sphere) const;
			void MakeLightBufferElement(LightTransformBuffer& outTransform, Params& outParam) const;

		public:
			virtual Core::Component* Clone() const;

		public:
			GET_ACCESSOR(Frustum, const Intersection::Frustum&, _frustum);
		};
	}
}