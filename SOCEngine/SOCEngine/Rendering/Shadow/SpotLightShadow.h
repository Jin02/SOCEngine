#pragma once

#include "BaseShadow.h"
#include "BaseLight.h"
#include "Transform.h"
#include "SpotLight.h"

namespace Rendering
{
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

		public:
			SpotLightShadow(BaseShadow base) : _base(base) {}
			ViewProjMatType MakeVPMatParam(const Light::LightPool<Light::SpotLight>& lightPool, const Core::TransformPool& tfPool);

			GET_CONST_ACCESSOR(ViewProjectionMatrix, const auto&, _transposedViewProjMat);
			GET_CONST_ACCESSOR(Base, const BaseShadow&, _base);

			GET_CONST_ACCESSOR(ObjectId, Core::ObjectId, _base.GetObjectId());

			inline void SetDirty(bool b) { _base.SetDirty(b); }
			GET_CONST_ACCESSOR(Dirty, bool, _base.GetDirty());

		private:
			Math::Matrix	_transposedViewProjMat;
			BaseShadow		_base;
		};
	}
}