#pragma once

#include "Sphere.h"
#include "Component.h"
#include "Color.h"
#include "Common.h"
#include "ShadowCommon.h"
#include "BoundBox.h"

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
			LightType		_type;
			float			_radius;
			Color			_color;
			uint			_lumen; //intensity

			bool			_useShadow;

			Math::Matrix	_viewMat;
			Math::Matrix	_viewProjMat;

		protected:
			LightForm();

		public:
			virtual ~LightForm();

		public:
			virtual void OnInitialize();
			virtual void OnDestroy();

		public:
			virtual void ComputeViewProjMatrix(const Intersection::BoundBox& sceneBoundBox) = 0;
			void ActiveShadow(bool isActive);

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

			uint Get32BitMainColor() const;

			GET_ACCESSOR(ViewMatrix,			const Math::Matrix&, _viewMat);
			GET_ACCESSOR(ViewProjectionMatrix,	const Math::Matrix&, _viewProjMat);
		};
	}
}