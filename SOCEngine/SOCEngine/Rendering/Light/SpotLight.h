#pragma once

#include "BaseLight.h"
#include "Half.h"
#include "Transform.h"

#include "Frustum.h"

namespace Rendering
{
	namespace Shadow
	{
		class SpotLightShadow;
	}

	namespace Manager
	{
		class LightManager;
	}

	namespace Light
	{
		namespace Buffer
		{
			class SpotLightBuffer;
		}

		class SpotLight final
		{
		public:
			using ManagerType		= Manager::LightManager;
			using LightBufferType	= Buffer::SpotLightBuffer;
			using TransformType		= Math::Vector4;
			using ShadowType		= Shadow::SpotLightShadow;

			struct Param
			{
				Half	dirX				= Half(0.0f);
				Half	dirY				= Half(0.0f);
				Half	outerConeCosAngle	= Half(0.0f);
				Half	innerConeCosAngle	= Half(0.0f);
			};

		public:
			explicit SpotLight(Core::ObjectID objID) : _base(objID) {};

			bool Intersect(const Intersection::Sphere& sphere, const Core::TransformPool& tfPool) const;
			bool Intersect(const Intersection::Frustum& frustum, const Core::TransformPool& tfPool) const;

			TransformType MakeTransform(const Core::Transform& transform) const;
			Param MakeParam(const Core::Transform& transform) const;
			inline void SetSpotAngleDegree(float d) { _spotAngleDegree = d; _dirty = true; }

			GET_ALL_ACCESSOR_PTR(Base,			BaseLight,		_base);
			GET_CONST_ACCESSOR(SpotAngleDegree, float,			_spotAngleDegree);
			GET_CONST_ACCESSOR(ObjectID,		Core::ObjectID,	_base.GetObjectID());

			GET_CONST_ACCESSOR(Dirty,	bool,	_dirty);
			SET_ACCESSOR(Dirty,			bool,	_dirty);

		private:
			BaseLight		_base;

			Param			_param;
			bool			_dirty				= true;
			float			_spotAngleDegree	= 0.0f;
		};
	}
}
