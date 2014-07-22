#pragma once

#include "Sphere.h"
#include "Component.h"
#include "Color.h"
#include "Common.h"

namespace Rendering
{
	namespace Light
	{
		class LightForm : public Core::Component
		{
		public:
			static const Component::Type GetComponentType() {	return Component::Type::Light;	}

		public:
			enum LightType{ DIRECTIONAL = 0, POINT, SPOT };

		protected:
			LightType _type;
		
		public:
			Color ambient;
			Color diffuse;
			Color specular;
		
			float range;
			float specularPower;

		protected:
			LightForm();

		public:
			virtual ~LightForm();

		public:
			virtual void Initialize();
			virtual void Destroy();

		public:
			virtual bool Intersects(const Intersection::Sphere &sphere) = 0;
			GET_ACCESSOR(LightType, const LightType, _type);
		};
	}
}