#pragma once

#include "BaseLight.h"
#include "Half.h"
#include "Transform.h"

namespace Rendering
{
	namespace Light
	{
		class SpotLight final
		{
		public:
			using TransformType = Math::Vector4;
			struct Param
			{
				Half	dirX				= Half(0.0f);
				Half	dirY				= Half(0.0f);
				Half	outerConeCosAngle	= Half(0.0f);
				Half	innerConeCosAngle	= Half(0.0f);
			};

		public:
			SpotLight(Core::ObjectId);

			bool Intersect(const Intersection::Sphere &sphere, const Core::Transform& transform) const;
			void MakeTransform(TransformType& outTransform, const Core::Transform& transform) const;
			void MakeParam(Param& outParam, const Core::Transform& transform);
			inline void SetSpotAngleDegree(float d) { _spotAngleDegree = d; _dirty = true; }

			GET_ACCESSOR(Base, BaseLight&, _base);
			GET_ACCESSOR(SpotAngleDegree, float, _spotAngleDegree);
			GET_CONST_ACCESSOR(ObjectId, const Core::ObjectId&, _base.GetObjectId());
			GET_SET_ACCESSOR(Dirty, bool, _dirty);

		private:
			BaseLight		_base;

			Param			_param;
			bool			_dirty = true;
			float			_spotAngleDegree = 0.0f;
		};
	}
}
