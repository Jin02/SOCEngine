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
			enum class LightType : uint { Directional = 0, Point, Spot };
			struct LightTransformBuffer
			{
				Math::Vector3	worldPosition;
				float			radius;
				LightTransformBuffer() : radius(0) {}
				~LightTransformBuffer() {}
			};

		protected:
			LightType	_type;
			float		_radius;
			Color		_color;

		protected:
			LightForm();

		public:
			virtual ~LightForm();

		public:
			virtual void OnInitialize();
			virtual void OnDestroy();

		public:
			virtual bool Intersects(const Intersection::Sphere &sphere) = 0;

		public:
			GET_ACCESSOR(Type, const LightType, _type);		

			GET_SET_ACCESSOR(Radius, float, _radius);
			GET_SET_ACCESSOR(Color, const Color&, _color);
		};
	}
}