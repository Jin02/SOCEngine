#pragma once

#include "Sphere.h"
#include "Component.h"
#include "Color.h"

namespace Rendering
{
	namespace Light
	{
		class LightForm : public Component
		{
		public:
			static const Component::Type ComponentType = Component::Type::Light;

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
			virtual bool Intersect(Collision::Sphere &sphere) = 0;
			Math::Vector3& GetWorldPosition();
			Math::Vector3& GetDirection();

		public:
			virtual void Initialize();
			virtual void Destroy();
			LightType GetType();
		};
	}
}