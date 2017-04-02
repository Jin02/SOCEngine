#pragma once

#include "BaseLight.h"
#include "Transform.h"

namespace Rendering
{
	namespace Light
	{
		class PointLight final
		{
		public:
			using TransformType = Math::Vector4;
			PointLight(Core::ObjectId id) : _base(id) {};

			bool Intersect(const Intersection::Sphere &sphere, const Core::Transform& transform) const;
			TransformType MakeTransform(const Core::Transform& transform) const;

			GET_ACCESSOR(Base, BaseLight&, _base);
			GET_CONST_ACCESSOR(ObjectId, const Core::ObjectId&, _base.GetObjectId());

		private:
			BaseLight					_base;
		};

	}
}
