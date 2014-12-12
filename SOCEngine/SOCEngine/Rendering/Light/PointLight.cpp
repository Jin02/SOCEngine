#include "PointLight.h"
#include "Object.h"

using namespace Intersection;

namespace Rendering
{
	namespace Light
	{
		PointLight::PointLight() : LightForm()
		{
			_type = LightType::Point;
		}

		PointLight::~PointLight(void)
		{
		}

		bool PointLight::Intersects(const Sphere &sphere)
		{
			Core::Transform* tf = _owner->GetTransform();
			Math::Vector3 wp;
			tf->WorldPosition(wp);

			return Sphere::Intersects(sphere, Sphere(wp, range));
		}

	}
}