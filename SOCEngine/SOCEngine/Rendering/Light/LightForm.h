#pragma once

#include "Sphere.h"
#include "Component.h"
#include "Color.h"
#include "Common.h"

#define MAXIMUM_LUMEN 12750

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
			uint		_lumen; //intensity

			Color		_shadowColor; //a is strength
			bool		_useShadow;

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

			void SetRadius(float r);
			GET_ACCESSOR(Radius, float, _radius);

			void SetColor(const Color& c);
			GET_ACCESSOR(Color, const Color&, _color);

			void SetLumen(uint l);
			GET_ACCESSOR(Lumen, uint, _lumen);

			void SetIntensity(float intensity);
			float GetIntensity() const;

			void SetShadowColor(const Color& c);
			GET_ACCESSOR(ShadowColor, const Color&, _shadowColor);

			GET_SET_ACCESSOR(UseShadow, bool, _useShadow);

			uint GetShderUintColor() const;

			GET_ACCESSOR(ShaderShadowUintColor, uint, _shadowColor.Get32BitUintColor());
		};
	}
}