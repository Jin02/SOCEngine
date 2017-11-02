#pragma once

#include "BaseLight.h"
#include "Transform.h"

namespace Rendering
{
	namespace Shadow
	{
		class PointLightShadow;
	}

	namespace Manager
	{
		class LightManager;
	}

	namespace Light
	{
		namespace Buffer
		{
			class PointLightBuffer;
		}

		class PointLight final
		{
		public:
			using ManagerType		= Manager::LightManager;
			using LightBufferType	= Buffer::PointLightBuffer;
			using TransformType		= Math::Vector4;
			using ShadowType		= Shadow::PointLightShadow;

			explicit PointLight(Core::ObjectID objID) : _base(objID) {};

			bool Intersect(const Intersection::Sphere &sphere, const Core::Transform& transform) const;
			TransformType MakeTransform(const Core::Transform& transform) const;

			GET_ALL_ACCESSOR(Base,			BaseLight&,		_base);
			GET_CONST_ACCESSOR(ObjectID,	Core::ObjectID,	_base.GetObjectID());

		private:
			BaseLight					_base;
		};

	}
}
