#pragma once

#include "BaseShadow.h"
#include "BaseLight.h"
#include "Transform.h"
#include "SpotLight.h"

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
			class SpotLightShadowBuffer;
		}

		class SpotLightShadow final
		{
		public:
			using ViewProjMatType	= Math::Matrix;
			using ShadowBufferType	= Buffer::SpotLightShadowBuffer;
			using LightType			= Light::SpotLight;
			using ManagerType		= Rendering::Manager::ShadowManager;

		public:
			SpotLightShadow(BaseShadow base) : _base(base) {}
			ViewProjMatType MakeVPMatParam(const Light::LightPool<Light::SpotLight>& lightPool, const Core::TransformPool& tfPool);

			GET_CONST_ACCESSOR(ViewProjectionMatrix,	const auto&,	_transposedViewProjMat);
			GET_CONST_ACCESSOR(Base,					const auto&,	_base);

			GET_CONST_ACCESSOR(ObjectID,				Core::ObjectID,	_base.GetObjectID());

			inline void SetDirty(bool b) { _base.SetDirty(b); }
			GET_CONST_ACCESSOR(Dirty, bool, _base.GetDirty());

		private:
			Math::Matrix	_transposedViewProjMat;
			BaseShadow		_base;
		};
	}
}