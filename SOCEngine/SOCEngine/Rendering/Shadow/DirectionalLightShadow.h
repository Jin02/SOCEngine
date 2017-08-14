#pragma once

#include "BaseShadow.h"
#include "BaseLight.h"
#include "Transform.h"
#include "DirectionalLight.h"

namespace Rendering
{
	namespace Manager
	{
		class ShadowManager;
	}

	namespace Shadow
	{
		namespace Buffer
		{
			class DirectionalLightShadowBuffer;
		}

		class DirectionalLightShadow final
		{
		public:
			using ViewProjMatType	= Math::Matrix;
			using ShadowBufferType	= Buffer::DirectionalLightShadowBuffer;
			using LightType			= Light::DirectionalLight;
			using ManagerType		= Rendering::Manager::ShadowManager;

			struct Param
			{
				float invProj_34 = 1.0f;
				float invProj_44 = 1.0f;
			};

		public:
			DirectionalLightShadow(BaseShadow base) : _base(base) {}
			ViewProjMatType MakeVPMatParam(const Light::LightPool<Light::DirectionalLight>& lightPool, const Core::TransformPool& tfPool, const Intersection::BoundBox& sceneBoundBox);

			GET_CONST_ACCESSOR(ViewProjectionMatrix, const auto&, _transposedViewProjMat);
			GET_CONST_ACCESSOR(Param, Param, _param);
			GET_CONST_ACCESSOR(Base, const BaseShadow&, _base);

			GET_SET_ACCESSOR(ProjectionSize, float, _projectionSize);
			GET_SET_ACCESSOR(UseAutoProjectionLocation, bool, _useAutoProjectLocation);

			GET_CONST_ACCESSOR(ObjectId, Core::ObjectId, _base.GetObjectId());

			inline void SetDirty(bool b) { _base.SetDirty(b); }
			GET_CONST_ACCESSOR(Dirty, bool, _base.GetDirty());

		public:
			constexpr static float FrustumMinZ = 1.0f;
			constexpr static float FrustumMaxZ = 10000.0f;

		private:
			BaseShadow		_base;
			Math::Matrix	_transposedViewProjMat;
			Param			_param;

			float			_projectionSize = 0.0f;
			bool			_useAutoProjectLocation = false;
		};
	}
}