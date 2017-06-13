#pragma once

#include "BaseLight.h"
#include "Transform.h"
#include "Half.h"

namespace Rendering
{
	namespace Shadow
	{
		class DirectionalLightShadow;
	}

	namespace Light
	{
		namespace LightingBuffer
		{
			class DirectionalLightingBuffer;
		}

		class DirectionalLight final
		{
		public:
			using LightingBufferType	= LightingBuffer::DirectionalLightingBuffer;
			using ShadowType			= Shadow::DirectionalLightShadow;

			struct TransformType
			{
				Half x = Half(0.0f), y = Half(0.0f);
				TransformType() = default;
				TransformType(Half _x, Half _y) : x(_x), y(_y) {}
			};
			explicit DirectionalLight(Core::ObjectId objId, LightId lightId) : _base(objId, lightId) {};

			void UpdateFlag(const Core::Transform& transform);

			TransformType MakeTransform(const Core::Transform& transform) const;
			bool Intersect(const Intersection::Sphere &sphere, const Core::Transform& transform) const { return true; }

		public:
			GET_CONST_ACCESSOR(LightShaftSize, float,_base.GetRadius());
			inline void SetLightShaftSize(float f) { _base.SetRadius(f); }

			GET_ACCESSOR(Base, BaseLight&, _base);
			GET_CONST_ACCESSOR(Base, const BaseLight&, _base);

			GET_CONST_ACCESSOR(ObjectId, Core::ObjectId, _base.GetObjectId());
			GET_CONST_ACCESSOR(LightId, LightId, _base.GetLightId());

		private:
			BaseLight					_base;
		};
	}
}
