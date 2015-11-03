#pragma once

#include "Sphere.h"
#include "Component.h"
#include "Color.h"
#include "Common.h"
#include "ShadowCommon.h"
#include "BoundBox.h"
#include <functional>

#define MAXIMUM_LUMEN 12750

namespace Rendering
{
	namespace Light
	{
		class LightForm : public Core::Component
		{
		public:
			friend class Shadow::ShadowCommon;

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
			Math::Matrix			_viewMat;
			Math::Matrix			_viewProjMat;

			Color					_color;
			LightType				_type;
			float					_radius;
			uint					_lumen; //intensity
			float					_projNear;

			Shadow::ShadowCommon*	_shadow;

		protected:
			LightForm();

		public:
			virtual ~LightForm();

		public:
			virtual void OnInitialize();
			virtual void OnDestroy();

		public:
			virtual void ComputeViewProjMatrix(const Intersection::BoundBox& sceneBoundBox) = 0;
			virtual void CreateLightShadow(const std::function<void()>& addUpdateCounter) = 0;
			void ActiveShadow(bool isActive);

		private:
			void AddOwnerUpdateCounter();

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

			GET_ACCESSOR(ViewMatrix,			const Math::Matrix&,	_viewMat);
			GET_ACCESSOR(ViewProjectionMatrix,	const Math::Matrix&,	_viewProjMat);

			GET_ACCESSOR(UseShadow, bool, _shadow != nullptr);
			GET_ACCESSOR(ShadowColor, const Color&, _shadow->GetColor());

			GET_SET_ACCESSOR(ProjectionNear, float, _projNear);
		};
	}
}