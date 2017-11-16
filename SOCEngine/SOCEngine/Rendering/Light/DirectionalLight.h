#pragma once

#include "BaseLight.h"
#include "Transform.h"
#include "Half.h"

#include "Frustum.h"

namespace Rendering
{
	namespace Shadow
	{
		class DirectionalLightShadow;
	}

	namespace Manager
	{
		class LightManager;
	}

	namespace Light
	{
		namespace Buffer
		{
			class DirectionalLightBuffer;
		}

		class DirectionalLight final
		{
		public:
			using ManagerType		= Manager::LightManager;
			using LightBufferType	= Buffer::DirectionalLightBuffer;
			using ShadowType		= Shadow::DirectionalLightShadow;

			struct TransformType
			{
				TransformType() = default;
				TransformType(Half _x, Half _y) : x(_x), y(_y) {}

				Half	x = Half(0.0f),
						y = Half(0.0f);
			};
			explicit DirectionalLight(Core::ObjectID objID) : _base(objID) {};

			void UpdateFlag(const Core::Transform& transform);

			TransformType MakeTransform(const Core::Transform& transform) const;
			bool Intersect(const Intersection::Sphere& sphere, const Core::TransformPool& tfPool) const { return true; }
			bool Intersect(const Intersection::Frustum& frustum, const Core::TransformPool& tfPool) const { return true; }

		public:
			GET_CONST_ACCESSOR(LightShaftSize,	float,			_base.GetRadius());
			GET_CONST_ACCESSOR(ObjectID,		Core::ObjectID, _base.GetObjectID());
			GET_ACCESSOR_REF(Base,								_base);

			inline void SetLightShaftSize(float f) { _base.SetRadius(f); }

		private:
			BaseLight					_base;
		};
	}
}
