#pragma once

#include "BaseLight.h"
#include "Half.h"
#include "Transform.h"

namespace Rendering
{
	namespace Light
	{
		namespace LightingBuffer
		{
			class SpotLightingBuffer;
		}

		class SpotLight final
		{
		public:
			using LightingBufferType	= LightingBuffer::SpotLightingBuffer;
			using TransformType			= Math::Vector4;

			struct Param
			{
				Half	dirX			= Half(0.0f);
				Half	dirY			= Half(0.0f);
				Half	outerConeCosAngle	= Half(0.0f);
				Half	innerConeCosAngle	= Half(0.0f);
			};

		public:
			explicit SpotLight(Core::ObjectId objId, LightId lightId) : _base(objId, lightId) {};

			bool Intersect(const Intersection::Sphere &sphere, const Core::Transform& transform) const;
			TransformType MakeTransform(const Core::Transform& transform) const;
			Param MakeParam(const Core::Transform& transform) const;
			inline void SetSpotAngleDegree(float d) { _spotAngleDegree = d; _dirty = true; }

			GET_ACCESSOR(Base, BaseLight&, _base);
			GET_CONST_ACCESSOR(Base, const BaseLight&, _base);

			GET_ACCESSOR(SpotAngleDegree, float, _spotAngleDegree);
			GET_CONST_ACCESSOR(ObjectId, Core::ObjectId, _base.GetObjectId());
			GET_CONST_ACCESSOR(LightId, LightId, _base.GetLightId());
			GET_SET_ACCESSOR(Dirty, bool, _dirty);

		private:
			BaseLight		_base;

			Param			_param;
			bool			_dirty = true;
			float			_spotAngleDegree = 0.0f;
		};
	}
}
