#include "SpotLight.h"
#include "Object.h"

using namespace Intersection;

namespace Rendering
{
	namespace Light
	{
		SpotLight::SpotLight()  : LightForm()
		{
			_type = LightType::Spot;
			spotAngle = 1.0f;
		}

		SpotLight::~SpotLight()
		{

		}

		bool SpotLight::Intersects(Intersection::Sphere &sphere)
		{
			//float angle = spotAngle;

			//if(angle < 1)				angle = 1;
			//else if(angle > 179)		angle = 179;

			//Math::Vector3 wp;
			//_owner->GetTransform()->WorldPosition(wp);
			//
			//Cone cone(angle, range, _owner->GetTransform()->GetForward(), wp);

			//return cone.Intersects(sphere);
			return false;
		}
	}
}