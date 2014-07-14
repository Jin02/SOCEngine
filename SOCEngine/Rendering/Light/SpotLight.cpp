#include "SpotLight.h"
#include "Transform.h"

using namespace Intersection;

namespace Rendering
{
	namespace Light
	{
		SpotLight::SpotLight()  : LightForm()
		{
			type = SPOT;
			spotAngle = 1.0f;
		}

		SpotLight::~SpotLight()
		{

		}

		bool SpotLight::Intersect(Intersection::Sphere &sphere)
		{
			float angle = spotAngle;

			if(angle < 1)				angle = 1;
			else if(angle > 179)		angle = 179;

			Cone cone(angle, range, ownerTransform->GetForward(), ownerTransform->GetWorldPosition());

			return cone.Intersection(sphere);
		}
	}
}