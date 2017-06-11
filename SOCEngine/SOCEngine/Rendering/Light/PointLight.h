#pragma once

#include "BaseLight.h"
#include "Transform.h"

namespace Rendering
{
	namespace Light
	{
		namespace LightingBuffer
		{
			class PointLightingBuffer;
		}

		class PointLight final
		{
		public:
			using LightingBufferType	= LightingBuffer::PointLightingBuffer;
			using TransformType			= Math::Vector4;

			explicit PointLight(Core::ObjectId objId, LightId lightId) : _base(objId, lightId) {};

			bool Intersect(const Intersection::Sphere &sphere, const Core::Transform& transform) const;
			TransformType MakeTransform(const Core::Transform& transform) const;

			GET_ACCESSOR(Base, BaseLight&, _base);
			GET_CONST_ACCESSOR(Base, const BaseLight&, _base);

			GET_CONST_ACCESSOR(ObjectId, Core::ObjectId, _base.GetObjectId());
			GET_CONST_ACCESSOR(LightId, LightId, _base.GetLightId());

		private:
			BaseLight					_base;
		};

	}
}
